//  MikeLite Shell v1-9.c
//  main.c
//  finalproj
//  Created by Mike on 6/01/16.
//  Copyright © 2016 Mike. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

// SET MASTER BUFFER SIZE
#define BUFFER_SIZE 1024
int BUFFER_AS_INT = BUFFER_SIZE;

// CONSTRUCT BUFFERS
char* parameterList[BUFFER_SIZE];
char* historyList[BUFFER_SIZE];
char CommandLineInput[BUFFER_SIZE];

/* FUNCTION DECLARATIONS */
void parseString(char inputLine[]);
void parseStringNoHist(char inputLine[]);
int has_quit(char string[]);
int execute();
char* historySave;
void allHist();
void nHist();
void errorThrow();
int global_count;
int histIndex;
int histMark;


/* MAIN SHELL LOOP */
int main(int argc, char** argv) {
    
    // init global count
    global_count = 0;
    
    printf("%s\n", " _____________________________________________________________" );
    printf("%s\n", "|.....////...////......////.....///////////////.//////////....|" );
    printf("%s\n", "|..../    //    /....../  /...../  /          /./        /....|" );
    printf("%s\n", "|..../          /.////./  /...../  /////  /////./   //////....|" );
    printf("%s\n", "|..../  //  //  /./  /./  /...../  /.../  /...../        /....|" );
    printf("%s\n", "|..../  /.//./  /.////./  /////./  /.../  /...../   //////....|" );
    printf("%s\n", "|..../  /..../  /....../      /./  /.../  /...../        /....|" );
    printf("%s\n", "|....////....////......////////.////...////.....//////////....|" );
    printf("%s\n", "|-------------------------------------------------------------|" );
    printf("%s\n", "|                     Mike Shell v1.9                         |" );
    printf("%s\n", "|                                                             |" );
    printf("%s\n", "|                 Operating Systems 575 SU '16                |" );
    printf("%s\n", "|_____________________________________________________________|" );
    printf("%s", "\n" );
    
    while(1){
        
        // Clear commandline input
        for(int i =0;i<BUFFER_SIZE;i++){
            CommandLineInput[i]= '\0';
            
        }
        
        // Line prompt set and print
        char* linePromt = "ME> ";
        fputs(linePromt, stdout);
        fflush(stdout);
    
        
        // Get user input, mark end
        if (fgets(CommandLineInput, sizeof CommandLineInput, stdin) != NULL ){
            char *nextln = strchr(CommandLineInput,'\n');
            if (nextln != NULL )
                *nextln = '\0';
        }
        
        
        // MAKE SURE BUFFER IS NOT EMPTY
        if (CommandLineInput[0] == '\0' || CommandLineInput[0] == ' '){
            continue;
        }
        else{
            // CHECK IF HISTORY COMMAND
            histMark = 0;
            if (CommandLineInput[0] == '!'){
                parseStringNoHist(CommandLineInput);
                global_count--;
                
                // !! EXEC LAST COMMAND HANDLING
                if(strcmp(parameterList[0],"!!") == 0){
                    char* linecopy;
                    linecopy = malloc(sizeof(char) * BUFFER_AS_INT);
                    linecopy = historyList[global_count];
                    parseStringNoHist(linecopy);
                }
                
                // !n HANDLING
                else if(parameterList[0][0] == '!' && (parameterList[0][1] >= '0' && parameterList[0][1] <= BUFFER_AS_INT)){
                    // parse number from command
                    for(int i = 0; i < BUFFER_SIZE; i++ ){
                        parameterList[0][i] = parameterList[0][i+1];
                    }
                    // take input and insert
                    
                    // atoi() is ASCII to int
                    histIndex = atoi(parameterList[0]);
                    char* linecopy;
                    linecopy = malloc(sizeof(char) * BUFFER_AS_INT);
                    linecopy = historyList[histIndex-1];
                    historySave = (char*) malloc(sizeof(linecopy));
                    strcpy(historySave,linecopy);
                    parseStringNoHist(linecopy);
                    histMark = 1;
                }
            }
            // ELSE IF NOT HISTORY COMMAND, PARSE NORMALLY
            else {
            parseString(CommandLineInput);
            }
        
            
        /*
         
        --------   COMMAND SELECTION   ----------
        IMPLEMENT YOUR ADDITIONAL BUILD-IN COMMANDS BELOW USING THE FOLLOWING SYNTAX
         
        if((strcmp(parameterList[0], "YOUR FUNCTION") == 0){
        Your execution block here for a hit
        global_count++; Increment the global count THIS IS IMPORTANT FOR INDEXING
        continue; Continue through next loop iteration
        }
    
        */
        // EXIT
        if((strcmp(parameterList[0], "quit") == 0) || (strcmp(CommandLineInput, "exit") ==0)){
            printf("Goodbye!\n");
            return(EXIT_SUCCESS);
        }
        
        // CD
        else if(strcmp(parameterList[0],"cd") == 0){
            chdir(parameterList[1]);
            global_count++;
            continue;
        }
        
        // DELETE
        else if(strcmp(parameterList[0],"delete") == 0){
            int retr;
            retr = remove(parameterList[1]);
            
            if(retr == 0){
                printf("%s", "SYS> Deleted\n");
            }
            else{
                errorThrow();
            }
            
            global_count++;
            continue;
        }
        
        // TOUCH
        else if(strcmp(parameterList[0],"touch") == 0){
            if (parameterList[1] != NULL) {
                fopen(parameterList[1], "wb");
            }
            else {
                errorThrow();
            }
            global_count++;
            continue;
        }

        // HISTORY
        else if(strcmp(parameterList[0],"history") == 0){
            global_count++;
            allHist();
            continue;
        }
        // HELP
        else if(strcmp(parameterList[0],"help")== 0){
            printf("\nWelcome to MIKE SHELL\n");
            printf("_______________________\n");
            printf("Built-ins: \n");
            printf("\'exit\' - Exit the shell \n");
            printf("\'cd\' - Change directory \n");
            printf("\'delete [file]\' - Delete file/folder \n");
            printf("\'touch [param]\' - Touch parameter, if non exists, create \n");
            printf("\'!!\' - Execute last command \n");
            printf("\'history\' - View input history \n");
            printf("\'!n\' - Execute n'th command (use history command first) \n");
            global_count++;
            continue;
        }
        
        // GO TO EXECUTE IF NONE HIT
        execute();
        global_count++;
            if (histMark == 1){
                // Correct records for skipped history
                historyList[histIndex-1] = historySave;
                histMark = 0;
            }
        }
    }
}

void parseString(char* inputLine){
    historyList[global_count] = (char*) malloc(sizeof(*inputLine));
    strcpy(historyList[global_count], inputLine);
    parameterList[0]= strtok(inputLine," ");
    for(int i=1; i < BUFFER_SIZE; i++){
        parameterList[i]= strtok (NULL, " ");
        if(parameterList[i]==NULL){
            break;
        }
    }
}

void parseStringNoHist(char* inputLine){
    parameterList[0]=strtok (inputLine," ");
    for(int i=1; i < BUFFER_SIZE; i++){
        parameterList[i]= strtok(NULL, " ");
        if( parameterList[i]== NULL){
            break;
        }
    }
}

int execute(){
    pid_t process_ID;
    process_ID = fork();
    pid_t wait_ID;
    int isRunning;
    // FORK ERROR
    if (process_ID < 0){
        errorThrow();
        return 0;
    }
    // CHILD PROCESS FORK
    else if (process_ID == 0){
        // Execute
        if (execvp(parameterList[0], parameterList) == -1){
            errorThrow();
        }
        exit(EXIT_SUCCESS);
    }
    else{
    // WAIT FOR CHILD
        do {
            // Wait on proccess: WUNTRACED->
            // The status of any child processes specified by pid that are stopped,
            // and whos status has not yet been reported since they stopped,
            // shall also be reported to the requesting process.
            wait_ID = waitpid(process_ID, &isRunning, WUNTRACED);
            // While there is no exited (WIFEXITED) or terminated by signal (WIFSIGNALED)
        } while (!WIFEXITED(isRunning) && !WIFSIGNALED(isRunning));
    }
    return 1;
}

// Genertic error throwing function
void errorThrow(){
    // Print the stored error code as text
    perror("SYS> Error: ");
}

// Print all the history
void allHist(){
    printf("%s "," ---- ALL HISTORY ---- \n") ;
    for(int i = 0, showNum = i; i < global_count ; i++, showNum++){
        printf("%s ", "Entry");
        printf("%d", i+1);
        printf("%s", ") ");
        printf("%s ", historyList[i]);
        printf("%s ", "\n");
    }
    printf("%s \n"," ---- COMPLETE ---- ") ;
}
