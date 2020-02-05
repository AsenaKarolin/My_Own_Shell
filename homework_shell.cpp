#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <fstream>
using namespace std;
#define READ_END	0
#define WRITE_END	1
int main(){
pid_t pid;
pid_t p;
char user[100000]; //c-string to get the username
string struser; //string to get the username
int des[2]; //array used for a pipe
int j; //variable to iterate through the footprint array
int fd; //file descriptor
int count=0; //variable that stores the number of commands that have been entered so far
int num; //variable used to calculate the associated number with each command when the "footprint" command is executed
string arr[15]; //array to keep the footprint
// forks and uses a pipe to read the user name using the "whoami" command
pipe(des);
p=fork();
if (p<0){
	//if fork() is unsuccessful, returns 1
	return 1;
}
else if (p==0){
	//child executes and writes the output of the "whoami" command on the "WRITE_END" of the pipe
	dup2(des[WRITE_END], 1);
	close(des[WRITE_END]);
	close(des[READ_END]);
	execlp("whoami","whoami",NULL);
}
else{
	//parent waits for child and then reads the output of the child from the "READ_END" of the pipe and prints it execpt for the last character which is "\n", this character should not be printed.
	wait(NULL);
	read(des[READ_END], user, sizeof(user));
	close(des[READ_END]);
	close(des[WRITE_END]);
	struser = user;
	struser = struser.substr(0,struser.size()-1);
}
//initializing the array for the "footprint" command
for (j=0; j<15; j++){
	arr[j]="asena";
}

while(1){
	//prints the username
	cout << struser << " >>> ";
	string str;
	//gets the input command
	getline(cin,str);
	//if command is "exit" exits the process
	if (str == "exit"){
		exit(0);
	}
	//count of commands is increased by 1
	count++;
	//the entered command is added to the array
	for (j=0; j<15; j++){
		if (arr[j]=="asena"){
			break;
		}
	}
	if (j==15){
		for (j=1; j<15; j++){
			arr[j-1]=arr[j];
		}
		arr[14]=str;
	}
	else{
		arr[j]=str;
	}
	//the process is forked
	pid = fork();
	if (pid < 0)
	//if the fork() failed, returns 1
	{
		return 1;
	}
	else if (pid == 0)
	//child executes
	{
	if (str == "listdir"){
	//if entered command is "listdir", executes the "ls" command
        	execlp("/bin/ls","ls",NULL);
	}
	else if (str.find("listdir | grep ")!=-1){
	//if entered command starts with "listdir | grep ", the string that is searched for is found
	string search = str.substr(str.find("\"") + 1, str.size()-str.find("\"") -2);
	pid_t pid2;
	int p1[2];
	//creates a pipe and forks again
	pipe(p1);
	pid2=fork();
	if (pid2<0){
		//if the fork() failed, returns 1
		return 1;
	}
	else if (pid2==0){
		//child executes
		//writes the output of the "ls" command on to the "WRITE_END" of the pipe
		dup2(p1[WRITE_END], 1);
		close(p1[WRITE_END]);
		close(p1[READ_END]);
		execlp("/bin/ls","ls",NULL);
	}
	else{
		//parent executes
		//reads the output of the "ls" command from the "READ_END" of the pipe and directs this output to STDIN using dup2(), than calls grep with the input string to search for
		wait(NULL);
		dup2(p1[READ_END], 0);
		close(p1[READ_END]);
		close(p1[WRITE_END]);
		execlp("/bin/grep", "grep",  search.c_str(), NULL);
	}
	}
	else if (str == "currentpath"){
		//if entered command is "currentpath", executes the "pwd" command
		execlp("/bin/pwd","pwd",NULL);
	}
	else if (str == "listdir -a"){
		//if entered command is "listdir -a", executes the "ls" command with the "-a" flag
		execlp("/bin/ls","ls","-a",NULL);
	}
	else if (str.find("listdir -a | grep ")!=-1){
	//if entered command starts with "listdir -a | grep ", the string that is searched for is found
	string search = str.substr(str.find("\"") + 1, str.size()-str.find("\"") -2);
	pid_t pid2;
	int p1[2];
	//creates a pipe and forks again
	pipe(p1);
	pid2=fork();
	if (pid2<0){
		//if the fork() failed, returns 1
		return 1;
	}
	else if (pid2==0){
		//child executes
		//writes the output of the "ls" command with the "-a" flag on to the "WRITE_END" of the pipe
		dup2(p1[WRITE_END], 1);
		close(p1[WRITE_END]);
		close(p1[READ_END]);
		execlp("/bin/ls","ls","-a",NULL);
	}
	else{
		//parent executes
		//reads the output of the "ls" command with the "-a" flag from the "READ_END" of the pipe and directs this output to STDIN using dup2(), than calls grep with the input string to search for
		wait(NULL);
		dup2(p1[READ_END], 0);
		close(p1[READ_END]);
		close(p1[WRITE_END]);
		execlp("/bin/grep", "grep",  search.c_str(), NULL);
	}
	}
	else if (str == "footprint"){
		//if entered command is "footprint", prints the command and its associated number. The commands are stored in the array and the number is found using the variable "count".
		for (j=0; j<15; j++){
			if (arr[j]=="asena"){
				break;
			}
			if (count>15){
				num=count-14+j;
			}
			else{
				num=j+1;
			}
			cout << num << " " << arr[j] << endl;
		}
		exit(0);
	}
	else if ((str.substr(0, str.find(" "))=="printfile") && (str.find(" > ")==-1)){
		//if entered command is "printfile" with no redirection, executes the "cat" command
		execlp("/bin/cat","cat",str.substr(str.find(" ")+ 1, str.size()-10).c_str(),NULL);
	}
	else if ((str.substr(0, str.find(" "))=="printfile") && (str.find(" >")!=-1)){
		//if entered command is "printfile" with redirection
		//opens the file, to which the output will be redirected, and clears it first
		fstream ofs;
		ofs.open(str.substr(str.find(">")+2,str.size()-12).c_str(), ios::out | ios::trunc);
		ofs.close();
		//gets the file descriptor of the file, to which the output will be redirected
		fd = open(str.substr(str.find(">")+2,str.size()-12).c_str(), O_RDWR|O_CREAT, 0777);
		//sets STDOUT as this file descriptor
		dup2(fd,1);
		//executes "cat" command
		execlp("/bin/cat","cat",str.substr(str.find(" ")+ 1, str.find(">") - 11).c_str(),NULL);
	}
	else{
		//if input is not valid, child exits without doing anything
		exit(0);	
	}
	}
	else
	{  
		//parent executes
		//parent waits for child to terminate
		wait(NULL);
	}
	
}
return 0;
}
