#include <SD.h> //Included SD Card Library


//Serial_Event_Test_12 Vaariables Begin
int MIS_hr = 0;
int MIS_min = 0;
int MIS_sec = 0;
String MIS_time = "00:00:00"; 

int GPS_hr = 0;
int GPS_min = 0;
int GPS_sec = 0;
//Serial_Event_Test_12 Variables End


//variables not initialized
float Pkg_Cum_Watts = 463.0f;
float Total_avg_Watts = 23.3f;


const int SD_pin = 53;

//variables to manage delay
unsigned long SD_start_time = millis();
unsigned long SD_wait_time = 3000;
unsigned long SD_target_time = millis()+SD_wait_time;

String SD_file_ext = ".txt";

String SD_report_file_name = "";
String SD_report_string = "";

String SD_GPS_file_name = "";
String SD_GPS_string = "";

String SD_misc_file_name = "";
String SD_misc_string = "";

String Data_msg_string = "";

int a = 1; //test line remember to remove it


/*
prepare data report to log in SD and sending HASP
*/
void prepare_report()
{
  String txt = "";
  txt += "MISSION TIME: " + get_time_string(GPS_hr,GPS_min,GPS_sec)+"\tPkg Cum Wh\t" +Total_avg_Watts;
}

void setup() {
  // Setting up serial port
  Serial.begin(9600);
  Serial.println("Intializing SD Card....");

  //check if SD is available
  if (!SD.begin(SD_pin))
  {
    Serial.println("SD Card Initialization Failed");
  }
  else
  {
  Serial.println("SD Card Initialization completed");
  }
}

/*check number and format for a string*/
String num_check(int num)
{
  if (num < 10)
    return "0"+(String)num;
  else
    return (String)num;
}

/*convert hr,min and sec to string*/
String get_time_string(int hour, int minute, int second)
{
  return num_check(hour)+":"+num_check(minute)+":"+num_check(second);
}


/*function to queue report message */
void log_report(String msg){
  SD_report_string += msg + "\n";
}

/*function to queue GPS message */
void log_GPS(String msg){
  SD_GPS_string += msg + "\n";
}

/*function to queue misc message */
void log_misc(String msg){
  SD_misc_string += msg + "\n";
}


/*
singular function to log data
filename : Name of the file for SD
msg : message to log
*/
void log_file_sd(String filename,String msg)
{
  File file = SD.open(filename, FILE_WRITE);
  if(file)
  {
    file.print(msg);
    Serial.print(msg); //printing on serial for testing, don't forget to remove it if not needed
    file.close();
  }
  else
  {
    Serial.println("Error in opening "+filename+" file");
  }
}


/*
Function to log report, GPS and misc data into sd file
*/
void log_sd()
{
  SD_report_file_name ="Report_" + num_check(MIS_hr) + SD_file_ext;
  SD_GPS_file_name = "GPS_"+ num_check(MIS_hr) + SD_file_ext;
  SD_misc_file_name = "Misc_"+ num_check(MIS_hr) + SD_file_ext;
  
  //Serial.println(SD_report_file_name);
  // put your main code here, to run repeatedly:

  log_report("Report file log at "+ (String)a++); //test line remember to remove it
  //log_report(get_time_string(1,12,34)); //test line remember to remove it
  //log_report(get_time_string(1,1,1)); //test line remember to remove it
  //log_report(get_time_string(0,0,45)); //test line remember to remove it
  log_file_sd(SD_report_file_name,SD_report_string);
  SD_report_string = "";
  
  log_GPS("GPS file log at "+ (String)a++); //test line remember to remove it
  log_file_sd(SD_GPS_file_name,SD_GPS_string);
  SD_GPS_string = "";

  log_misc("Misc file log at "+ (String)a++); //test line remember to remove it
  log_file_sd(SD_misc_file_name,SD_misc_string);
  SD_misc_string = "";

 
}





void loop() {

  if(SD_target_time <= millis())
  {
      prepare_report(); //prepare report
      log_sd(); //log to SD
      SD_target_time += SD_wait_time;
  }
}
