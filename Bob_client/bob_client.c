#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

#include<netinet/in.h>

int main(int argc, char** argv)
{
	int alice,bob;

	FILE *c_out, *enc_messages, *random, *message;

	//Creating socket Alice
	alice = socket(AF_INET, SOCK_STREAM, 0);

	//Setting necessary values for the server
	struct sockaddr_in server_alice;
	server_alice.sin_family=AF_INET;
	server_alice.sin_port=htons(atoi(argv[2]));
	server_alice.sin_addr.s_addr=inet_addr(argv[1]);
	bzero(&server_alice.sin_zero, 8);

	//Connecting to Alice
	if(connect(alice, (struct sockaddr*)&server_alice, sizeof(struct sockaddr_in)) == -1){
		printf("Connection could not be established!");
		exit(-1);
	}

	printf("Connection to Alice successful\n");

	//Buffer is used to send and recieve messages
	char *buffer;
	buffer=(char*)malloc(350*sizeof(char));

	//Recieves the random strings from alice and writes to randomx.txt
	random = fopen("randomx.txt", "w");

	recv(alice, buffer, 350, 0);
	fprintf(random,"%s",buffer);
	bzero(buffer, strlen(buffer));

	recv(alice, buffer, 350, 0);
	fprintf(random,"%s",buffer);
	bzero(buffer, strlen(buffer));

	fclose(random);
	printf("Randomly generated strings recieved from ALice\n");

	//Fork to execute bob1
	pid_t pid=fork();

	//child process
	if(pid==0)
	{
		//Execute bob1
		static char *argv[]={"bob1",NULL};
		execv("bob1", argv);
		exit(-1);
	}

	//Waits for child process to finish
	wait(NULL);

	//Sends the c generated by bob1 to alice
	c_out = fopen("bob_c.txt", "r");

	fscanf(c_out, "%s", buffer);
	send(alice, buffer, strlen(buffer), 0);
	bzero(buffer, strlen(buffer));
	
	fclose(c_out);
	printf("c sent to ALice\n");

	//receives the encrypted messages generated by Alice and stores in enc_messages.txt
	enc_messages = fopen("enc_messages.txt", "w");

	recv(alice, buffer, 350, 0);
	fprintf(enc_messages,"%s",buffer);
	bzero(buffer, strlen(buffer));

	//fprintf(enc_messages,"\n");

	recv(alice, buffer, 350, 0);
	fprintf(enc_messages,"%s",buffer);
	bzero(buffer, strlen(buffer));

	fclose(enc_messages);
	printf("Encrypted messages recieved from Alice\n");

	//Fork to execute bob2
	pid=fork();

	//child process
	if(pid==0)
	{
		//child process bob2
		static char *argv[]={"bob2",NULL};
		execv("bob2", argv);
		exit(-1);
	}

	//Waits for child process to finish
	wait(NULL);

	close(alice);
	printf("Transfer complete\n");

	message = fopen("bob_message.txt","r");
	fscanf(message,"%s", buffer);
	printf("Number recieved from ALice is: %s\n", buffer);
}