#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
    string line;
  string key;
  string value; 
  float memtot = 0.0;
  float memfree = 0.0;
  float buffer = 0.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::remove(line.begin(), line.end(), ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      //std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          //std::replace(value.begin(), value.end(), '_', ' ');
               memtot = std::stof(value);}
            else if(key == "MemFree")  { 
              memfree = std::stof(value); }
           else if(key == "Buffers") { 
             buffer = std::stof(value);
                    break;}
      }
    }
  }
  return (1- memfree/(memtot - buffer)); 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  try{
   return std::stol(uptime);}
   catch (const std::invalid_argument& arg) {
          return 0;
        }
}

// TODO: Read and return the number of jiffies for the system
//long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
//long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
//long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
//long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization() { 
  string line; 
  string key;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice; 
  float previdle    =   0.0; 
  float previowait  =   0.0; 
  float prevuser    =   0.0;
  float prevnice    =   0.0;
  float prevsystem  =   0.0;
  float previrq     =   0.0; 
  float prevsoftirq =   0.0;
  float prevsteal   =   0.0; 
  float Previdle    =   0.0;
  float PrevNonidle =   0.0;
  float Prevtotal   =   0.0; 
  float total       =   0.0;
  float Nonidle     =   0.0;
  float totald      =   0.0;
  float	idled       =   0.0;
  float Idle        =   0.0;
  float CPU_Percentage;
  //vector<string> cpu_values;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice)       {
        if(key == "cpu"){
          Previdle = previdle + previowait;
		  Idle =  std::stof(idle) +  std::stof(iowait);
		  PrevNonidle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
		  Nonidle =  std::stof(user) +  std::stof(nice) +  std::stof(system) +  std::stof(irq) + 		    std::stof(softirq) +  std::stof(steal);

		  Prevtotal = Previdle + PrevNonidle;
		  total     = Idle + Nonidle;

		//  # differentiate: actual value minus the previous one
		  totald = total - Prevtotal;
		  idled =  std::stof(idle) - Previdle;

		  CPU_Percentage = (totald - idled)/totald; 
          previdle    =   std::stof(idle); 
          previowait  =   std::stof(iowait); 
          prevuser    =   std::stof(user);
          prevnice    =   std::stof(nice);
          prevsystem  =   std::stof(system);
          previrq     =   std::stof(irq); 
          prevsoftirq =   std::stof(softirq);
          prevsteal   =   std::stof(steal); 
          return CPU_Percentage; 
        }
       }
     }
   }
  return 0.0;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
   string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          try{ return std::stoi(value);}
          catch (const std::invalid_argument& arg) { return 0;}
        }
      }
    }
  }
  return 0;
 }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_runnning") {
				try { return std::stoi(value);}
			    catch (const std::invalid_argument& arg) {return 0;}
        }
      }
    }
  }
  return 0;
 }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid)+ kCmdlineFilename);
  if (filestream.is_open()) {
     std::getline(filestream, line) ;
     }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          try{ return std::to_string((std::stof(value))/1000.0);}
          catch (const std::invalid_argument& arg) {return 0;}
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return value;
 }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  
  string uid = Uid(pid); 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == uid) {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  string value;
  string key;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
	  for (int i = 1; i <22; i++){  
          linestream >> value;
      }
      linestream>>key; 
      try{ return (LinuxParser::UpTime() - (std::stol(key)/sysconf(_SC_CLK_TCK)));}
          catch (const std::invalid_argument& arg) {return 0;}
   }
  return 0;
}