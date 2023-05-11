#include<iostream>
#include<fstream>
#include<sys/inotify.h>   // the notification event
#include<unistd.h>
#include<string>
using namespace std;

// IMPORTANT: 
// change all occurences in line 29 and 40 of "/home/afraz/OperatingSystemsLab/OSProject-daemons" with your
// working directory

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024* (EVENT_SIZE + 16))

void LogEvent(string event){
	ofstream log_file("directory.log", ios_base::app);
	if(log_file.is_open()){
		log_file << event << endl;
		log_file.close();
	} else {
		cerr << "Error: Can not open log file" << endl;
	}
}

int main(){
	int fd, wd;
	char buffer[EVENT_BUF_LEN];
	
	// Change the working directory to where the executable is located
	
	if(chdir("/home/afraz/OperatingSystemsLab/OSProject-daemons") < 0){
		cerr << "Error: Can not change working directory" << endl;
	}
	
	// we are watching through a notification. this is its notify event.
	fd = inotify_init();
	if(fd < 0){
		cerr << "Error: Can not initialize the notify." << endl;
	}
	
	// start the process of watching the directory.
	wd = inotify_add_watch(fd, "/home/afraz/OperatingSystemsLab/OSProject-daemons", IN_CREATE | IN_DELETE);
	if(wd < 0){
		cerr << "Error: Can not watch the directory." << endl;
	}
	
	// Fork to create a daemon process
	pid_t pid = fork();
	if(pid < 0){
		cerr << "Error: Can not Fork" << endl;
	}else if (pid > 0){
		return 0; // Exit the daemon process
	}
	
	// Build the loop which powers the daemon
	// This is the loop that is going to watch changes in the file
	 while(true){
	 	int length = read(fd, buffer, EVENT_BUF_LEN);
	 	if(length < 0){
	 		cerr << "Error: Can not read the directory changes." << endl;
	 		return 1;
	 	}
	 	
	 	int i = 0;
	 	
	 	while(i < length){
	 		struct inotify_event *event = (struct inotify_event *) &buffer[i];
	 		if(event->len){
	 			if(event->mask & IN_CREATE){
	 				string nameEvent = "File Created: "+ string(event->name);
	 				LogEvent(nameEvent);
	 			}else if(event->mask & IN_DELETE){
	 				string nameEvent = "File Deleted: "+ string(event->name);
	 				LogEvent(nameEvent);
	 			}
	 		} 
	 		
	 		i += EVENT_SIZE + event->len;
	 	}
	 }
	
	return 0; // unreachable code
}
