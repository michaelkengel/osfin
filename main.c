//
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

/* FUNCTION DECLARATIONS */
void parseString(char inputLine[]);
void parseStringNoHist(char inputLine[]);
int has_quit(char string[]);
int execute();
void allHist();
void nHist();
void errorThrow();
void cleanHist();
int global_count;

/* MAIN SHELL LOOP */
int main(int argc, char** argv) {
    
    // init global count
    global_count = 0;
    
    char CommandLineInput[BUFFER_SIZE];
    
    while(1){
        
        // Make sure all unused history is null terminator
        cleanHist();
        
        // Clear commandline input
        for(int i =0;i<BUFFER_SIZE;i++){
            CommandLineInput[i]= '\0';
        }
        
        // Line prompt set and print
        char* linePromt = "ME> ";
        fputs(linePromt, stdout);
        fflush(stdout);
        
        if (fgets(CommandLineInput, sizeof CommandLineInput, stdin) != NULL ){
            char *nextln = strchr(CommandLineInput,'\n');
            if (nextln != NULL )
                *nextln = '\0';
        }
        
        // Blank space entered bug fix
        if(CommandLineInput[0] == ' '){
            continue;
        }
        
        // Make sure there is smething in buffer other than null term
        if(CommandLineInput[0] != '\0'){
            parseString(CommandLineInput);
        
        jumpList:
            
            for(int index=0; (index < BUFFER_SIZE) && !(parameterList[index]==NULL); index++){
                // Just walk
            }
            for(int index=0; (index < BUFFER_SIZE) && !(historyList[index] == NULL); index++){
                // Just walk
            }
            
            // EXIT
            if((strcmp(parameterList[0], "quit") == 0) || (strcmp(CommandLineInput, "exit") ==0)){
                printf("Goodbye! \n");
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
                if (parameterList[1] != NULL) {
                    if (remove(parameterList[1])){
                        printf("%s ", "Error deleting file");
                    }
                    else{
                        printf("%s ", "Sucsess");
                    }
                }
                else{
                    printf("Command error - parameters missing.");
                }
                global_count++;
                continue;
            }
            // TOUCH
            else if(strcmp(parameterList[0],"touch") == 0){
                if (parameterList[1] != NULL) {
                    FILE* createdFile;
                    fopen(parameterList[1], "wb");
                    printf("hit");
                    if (createdFile != NULL) {
                        fclose(createdFile);
                    }
                }
                else{
                    printf("Command error - parameters missing.");
                }
                global_count++;
                continue;
            }
            // !! EXECUTE LAST
            else if(strcmp(parameterList[0],"!!") == 0){
                char* linecopy;
                linecopy = malloc(sizeof(char) * BUFFER_AS_INT);
                linecopy = historyList[global_count-1];
                parseStringNoHist(linecopy);
                goto jumpList;
            }
            // HISTORY
            else if(strcmp(parameterList[0],"history") == 0){
                allHist();
                global_count++;
                continue;
            }
            // !8
            else if(parameterList[0][0] == '!' && (parameterList[0][1] >= '0' && parameterList[0][1] <= BUFFER_AS_INT)){
                // parse number from command !87
                for(int i = 0; i < BUFFER_SIZE; i++ ){
                    parameterList[0][i] = parameterList[0][i+1];
                }
                // take input and insert
                int k;
                k = atoi(parameterList[0]);
                char* linecopy;
                linecopy = malloc(sizeof(char) * BUFFER_AS_INT);
                linecopy = historyList[k-1];
                parseStringNoHist(linecopy);
                goto jumpList;
            }
            // HELP
            else if(strcmp(CommandLineInput,"help")== 0){
                printf("\nWelcome to MIKE SHELL\n");
                printf("_______________________\n");
                printf("Built-ins: \n");
                printf("\'Exit\': Exit the shell \n");
                printf("\'cd\': Change directory \n");
                printf("\'delete [file]\': Delete file/folder \n");
                printf("\'touch [param]\': Touch parameter, if non exists, create \n");
                printf("\'!!\': Execute last command \n");
                printf("\'History\': View input history \n");
                global_count++;
                continue;
            }
            execute();
            global_count++;
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
        parameterList[i]= strtok (NULL, " ");
        if( parameterList[i]== NULL){
            break;
        }
    }
}

// NEED TO CHANGE
int execute(){
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0){ // Child process
        if (execvp(parameterList[0], parameterList) == -1){
            errorThrow();
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0){ // Fork error
            errorThrow();
    }
    else{ // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

void errorThrow(){
    // Print the error
    perror("Error: ");
}

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

void cleanHist(){
    for(int i = global_count; i < BUFFER_SIZE; i++){
        historyList[i] = '\0';
    }
}

