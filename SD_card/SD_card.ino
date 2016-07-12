#include <SD.h> //Included SD Card Library


//Serial_Event_Test_12 Vaariables Begin
int MIS_hr = 21;
int MIS_min = 56;
int MIS_sec = 34;

int GPS_hr = 20;
int GPS_min = 15;
int GPS_sec = 30;
float GPS_alt = 125000;
float pressure = 300.3f;
float tempTBay = -22.6f;
float tempEBay = -10.3f;
float tempOut = -53.0;
float pwr[6] = {3.0f,3.2f,2.3f,1.8f,1.7f,7.7f};
int temp[6] = {-20,-23,-21,-22,-20,-11};
int count[6] = {43,46,33,26,22,63};

int TBay_Tgt_Temp = -22;
int EBay_Tgt_Temp = -10;
int blinkCount = 9;

boolean HeaterFail[6] = {false,false,false,false,false,false};
boolean own_GPS_Fail = false;
boolean arduino_Fail = false;
//Serial_Event_Test_12 Variables End


//Servo variables start
boolean ServoFailFlagArray[8] = {false,true,false,false,false,false,false,false};
//Servo variables end

//variables not initialized
float Pkg_Cum_Watts = 463.0f;
float Total_avg_Watts = 23.2f;
float Volts30V = 33.0f, Volts6V = 5.41f;
//end variable not initialized

const int SD_pin = 53;

//variables to manage delay
unsigned long SD_start_time = millis();
unsigned long SD_wait_time = 5000;
unsigned long SD_target_time = millis() + SD_wait_time;

String SD_file_ext = ".txt";

String SD_report_file_name = "";
String SD_report_string = "";

String SD_GPS_file_name = "";
String SD_GPS_string = "";

String SD_misc_file_name = "";
String SD_misc_string = "";

String Data_msg_string = "";

int a = 1; //test line remember to remove it




void setup() {
  // Setting up serial port
  Serial.begin(4800);
 // Serial.println("Intializing SD Card....");

  //check if SD is available
//  if (!SD.begin(SD_pin))
//  {
//    Serial.println("SD Card Initialization Failed");
//  }
//  else
//  {
//    Serial.println("SD Card Initialization completed");
//  }
}

/*check number and format for a string*/
String num_check(int num)
{
  if (num < 10)
    return "0" + (String)num;
  else
    return (String)num;
}

/*convert hr,min and sec to string*/
String get_time_string(int hour, int minute, int second)
{
  return num_check(hour) + ":" + num_check(minute) + ":" + num_check(second);
}


/*function to queue report message */
void log_report(String msg) {
  SD_report_string += msg + "\n";
}

/*function to queue GPS message */
void log_GPS(String msg) {
  SD_GPS_string += msg + "\n";
}

/*function to queue misc message */
void log_misc(String msg) {
  SD_misc_string += msg + "\n";
}


/*
  singular function to log data
  filename : Name of the file for SD
  msg : message to log
*/
void log_file_sd(String filename, String msg)
{
  File file = SD.open(filename, FILE_WRITE);
  if (file)
  {
    file.print(msg);
    Serial.print(msg); //printing on serial for testing, don't forget to remove it if not needed
    file.close();
  }
  else
  {
    Serial.println("Error in opening " + filename + " file");
  }
}


/*
  Function to log report, GPS and misc data into sd file
*/
void log_sd()
{
  SD_report_file_name = "Report_" + num_check(MIS_hr) + SD_file_ext;
  SD_GPS_file_name = "GPS_" + num_check(MIS_hr) + SD_file_ext;
  SD_misc_file_name = "Misc_" + num_check(MIS_hr) + SD_file_ext;

  //Serial.println(SD_report_file_name);
  // put your main code here, to run repeatedly:

  log_report("Report file log at " + (String)a++); //test line remember to remove it
  //log_report(get_time_string(1,12,34)); //test line remember to remove it
  //log_report(get_time_string(1,1,1)); //test line remember to remove it
  //log_report(get_time_string(0,0,45)); //test line remember to remove it
  log_file_sd(SD_report_file_name, SD_report_string);
  SD_report_string = "";

  log_GPS("GPS file log at " + (String)a++); //test line remember to remove it
  log_file_sd(SD_GPS_file_name, SD_GPS_string);
  SD_GPS_string = "";

  log_misc("Misc file log at " + (String)a++); //test line remember to remove it
  log_file_sd(SD_misc_file_name, SD_misc_string);
  SD_misc_string = "";


}

/*
  prepare data report to log in SD and sending HASP
*/
void prepare_report()
{
  String msg = "";
  msg += "\u0001" + (String)"6," + get_time_string(GPS_hr, GPS_min, GPS_sec) + ",";
  String txt = "";
  String fail_msg = "FAILURE DATA (1=FAIL)\nSERVO DRIVERS: ";
  String s_fail = "SERVOS: ", h_fail = "HEATERS: ";
  String temp_st, count_st, pwr_st;
  float pwr_total = 0.0f;

  for (int i = 0; i < 5; i++)
  {
    s_fail += ServoFailFlagArray[i + 1] ? "1" : "0";
    h_fail += HeaterFail[i] ? "1" : "0";
    pwr_total += pwr[i];
    temp_st += (String)temp[i] + "\t";
    count_st += (String)count[i] + "\t";
    pwr_st += (String)pwr[i] + "\t";
  }
  s_fail += (String)(ServoFailFlagArray[6] ? "1" : "0") + (ServoFailFlagArray[7] ? "1" : "0") + "\n";
  h_fail += (String)(HeaterFail[5] ? "1" : "0") + "\n";
  temp_st += (String)temp[5] + "\t<-- Temp Deg. C\n";
  count_st += (String) count[5] + "\t <-- DUTY CYCLE % (10 min avg)\n";
  pwr_st += (String)pwr[5] + "\t <-- Avg. Watts\n";
  fail_msg += (String) (ServoFailFlagArray[0] ? "1" : "0") + "\n"
              + s_fail + h_fail + "GPS: "
              + (own_GPS_Fail ? "1" : "0") + "\nARDUINO: "
              + (arduino_Fail ? "1" : "0") + "\n";



  txt += "MISSION TIME: " + get_time_string(MIS_hr, MIS_min, MIS_sec) + "\t\tPkg Cum Wh:" + Pkg_Cum_Watts + "\n" +
         "VOLTAGES (BATT, PS2): " + Volts30V + ", " + Volts6V + "\n"
         + "ALTITUDE:" + GPS_alt + "\tPRESSURE: " + pressure + " mb\n\n\t\t"
         + "TBAY\tEBAY\tOUTSIDE\n"
         + "Temp\t\t" + tempTBay+"\t"+tempEBay+"\t"+tempOut+"\n"
         + "Temp_Tgt\t"+ TBay_Tgt_Temp + "\t" + EBay_Tgt_Temp + "\t" + blinkCount + "\t<-- BLINK\n"
         + "Avg_Watts\t" + pwr_total + "\t" + pwr[5] + "\t" + Total_avg_Watts + "\t<-- TOTAL\n\n"
         + "BOW\tSTBD\tSTRN\tPORT\tTOP\tEBAY\t<-- HEATERS\n" + temp_st + count_st + pwr_st + "\n\n"
         + "S#\tCMD\tPOS\tERR\tTIME\t%BSLN\tJOULES\t%BSLN\n";

  //add servo report

  txt += "\n" + fail_msg + "\n";

  byte check_sum = 0;
  for (char c:txt)
  {
    check_sum += (byte)c;
  }

  msg += (String) txt.length() + "\n" + "\u0002" + txt + "\u0003" + check_sum + "\n" + "\u0004";


  Serial.println(msg + "\n\n\n\n");
}



void loop() {

//Serial.println(prepare_report());


  if (SD_target_time <= millis())
  {
    prepare_report(); //prepare report
    //log_sd(); //log to SD
    SD_target_time += SD_wait_time;
  }
}
