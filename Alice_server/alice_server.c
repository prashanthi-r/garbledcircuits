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
#define BUFFER_SIZE 350

int main(int argc, char** argv)
{
	int bob, alice, len, input;
	pid_t pid;

	FILE *ciph, *hash, *rndm, *bob_c, *enc_mes, *alabel, *blabel, *initvec;
	alabel = fopen("alabel.txt", "r+");
	blabel = fopen("blabel.txt", "r+");
	
	//variables for reading encryptions from the file "ciph.txt"
	char *e1, *e2, *e3, *e4, *h_op0, *h_op1, *iniv; //h_op0 is the hashed encrypted output 0
	int n1, n2, n3, n4, *l1, *l2, *l3, *l4;
	l1 = (int*)malloc(sizeof(int));
	l2 = (int*)malloc(sizeof(int));
	l3 = (int*)malloc(sizeof(int));
	l4 = (int*)malloc(sizeof(int));


	//variables keys
	char *a_0, *b_0, *a_1, *b_1, *x_0, *x_1, *c; 
	a_0 = (char*) malloc(300*sizeof(char));
	b_0 = (char*) malloc(300*sizeof(char));
	a_1 = (char*) malloc(300*sizeof(char));
	b_1 = (char*) malloc(300*sizeof(char));
	x_0 = (char*) malloc(300*sizeof(char));
	x_1 = (char*) malloc(300*sizeof(char));
	c = (char*) malloc(300*sizeof(char));



	//Creation of socket Alice
	alice = socket(AF_INET, SOCK_STREAM, 0);


	//Setting necessary values for the server
	struct sockaddr_in server_alice;
	struct sockaddr_in server_bob;
	server_alice.sin_family=AF_INET;
	server_alice.sin_port=htons(atoi(argv[1]));
	server_alice.sin_addr.s_addr=INADDR_ANY;
	bzero(&server_alice.sin_zero, 8);


	//Binding socket Alice to the server values set for alice_server
	if((bind(alice, (struct sockaddr*)&server_alice, sizeof(struct sockaddr_in))) == -1){
		printf("Server binding unsuccessful!\n");
		exit(-1);
	}


	//Server listens to just one connection at a time
	if((listen(alice, 1)) == -1){
		printf("Listen unsuccessful!\n");
		exit(-1);
	}
	else
		printf("Alice is listening...\n");


	//len contains size of structure sockaddr_in
	len=sizeof(struct sockaddr_in);


	//Accepts incoming connection request
	if((bob=accept(alice, (struct sockaddr *)&server_bob, &len)) < 0){
		printf("Unable to accept connection!\n");
		exit(-1);
	}
	else
		printf("Alice has accepted a connection...\n");


	//Buffer is used to send and recieve messages
	char *buffer;
	buffer = (char*)malloc(BUFFER_SIZE*sizeof(char));


	//Fork to execute al_gate
	pid = fork();

	//child process
	if(pid == 0)
	{
		//child process al_gate
		static char *argv[] = {"al_gate", NULL};
		execv("al_gate", argv);
		exit(-1);
	}

	//Waits for al_gate to finish
	wait(NULL);


/**********************************************************/

	printf("Sending initialisation vector...\n");

	iniv = (char*)malloc(20*sizeof(char));
	initvec = fopen("initvec.txt", "r");
	fscanf(initvec, "%s", iniv);
	strcat(iniv, "\n");
	send(bob, iniv, strlen(iniv), 0);    //A0
	bzero(iniv, strlen(iniv));


/*********************************************************/

	printf("Sending the garbled table...\n");

	//Send garbled table generated by alice_gate to Bob
	
	/*************here, we send e1, e2, e3, e4 in the same order****************************************************/

	ciph = fopen("ciph.txt", "r");
	
	fscanf(ciph, "%d\n", l1);	
	n1 = (int)(*l1);
	e1 = (char*)malloc((n1+2)*sizeof(char));
	for (int i = 0; i < n1; ++i)
	{
		e1[i] = fgetc(ciph);
	}
	strcat(e1, "\n");
	send(bob, e1, strlen(e1), 0);   //A1
	//bzero(e1, strlen(e1));

	fscanf(ciph, "%d\n", l2);	
	n2 = (int)(*l2);
	e2 = (char*)malloc((n2+2)*sizeof(char));
	for (int i = 0; i < n2; ++i)
	{
		e2[i] = fgetc(ciph);
	}		
	strcat(e2, "\n");
	send(bob, e2, strlen(e2), 0);   //A2
	//bzero(e2, strlen(e2));

	fscanf(ciph, "%d\n", l3);	
	n3 = (int)(*l3);
	e3 = (char*)malloc((n3+2)*sizeof(char));
	for (int i = 0; i < n3; ++i)
	{
		e3[i] = fgetc(ciph);
	}	
	strcat(e3, "\n");
	send(bob, e3, strlen(e3), 0);    //A3
	//bzero(e3, strlen(e3));

	fscanf(ciph, "%d\n", l4);	
	n4 = (int)(*l4);
	e4 = (char*)malloc((n4+2)*sizeof(char));
	for (int i = 0; i < n4; ++i)
	{
		e4[i] = fgetc(ciph);
	}	
	strcat(e4, "\n");
	send(bob, e4, strlen(e4), 0);     //A4
	//bzero(e4, strlen(e4));

	fclose(ciph);
	printf("Alice has sent garbled table to Bob.\n");

	//Sending the hashes of op_0 and op_1 to Bob
	hash = fopen("hash.txt", "r");

	h_op0 = (char*)malloc(42*sizeof(char));
	h_op1 = (char*)malloc(42*sizeof(char));
	for (int i = 0; i < 39; ++i)
	{
		h_op0[i] = fgetc(hash);
	}

	fscanf(hash, "\n");
	for (int i = 0; i < 39; ++i)
	{
		h_op1[i] = fgetc(hash);
	}

	printf("Sending the hashes of encrypted outputs...\n");

	strcat(h_op0, "\n");
	send(bob, h_op0, strlen(h_op0), 0);    //A5

	strcat(h_op1, "\n");
	send(bob, h_op1, strlen(h_op1), 0);     //A6

/**************************************************************************************************************/

	printf("Sending keys to Bob...\n");
	//Sending Bob Alice's input labels

	printf("Enter your input for gate-1: ");
	scanf("%d", &input);	
	fscanf(alabel, "%s", a_0);
	fscanf(alabel, "\n");
	fscanf(alabel, "%s", a_1);
	printf("a0: %s\n", a_0);
	fscanf(blabel, "%s", b_0);
	fscanf(blabel, "\n");
	fscanf(blabel, "%s", b_1);
	if(input == 0){
		strcat(a_0, "\n");
		send(bob, a_0, strlen(a_0), 0);    //A7
	}
	else{
		strcat(a_1, "\n");
		send(bob, a_1, strlen(a_1), 0);    //A7
	}

/***********************************Oblivious Transfer for sending b0/b1*******************************************/

	//Fork to execute alice1
	pid = fork();

	//child process
	if(pid == 0)
	{
		//Execute alice1
		static char *argv[] = {"alice1", NULL};
		execv("alice1", argv);
		exit(-1);
	}

	//Waits for execution of alice1 to finish
	wait(NULL);

	//Send randomly generated nymbers by alice1 to bob
	rndm = fopen("randomx.txt", "r");

	fscanf(rndm, "%s", x_0);
	strcat(x_0, "\n");
	send(bob, x_0, strlen(x_0), 0);    //A8

	fscanf(rndm, "%s", x_1);
	send(bob, x_1, strlen(x_1), 0);    //A9

	fclose(rndm);
	
	printf("Alice has sent randomly generated numbers\n");
	
	//Recieve the c generated by Bob and store in bob_c.txt
	bob_c = fopen("bob_c.txt", "w");

	recv(bob, c, 350, 0);    //B1
	fprintf(bob_c, "%s", c);
	bzero(c, strlen(c));

	fclose(bob_c);
	printf("Alice has recieved c from Bob\n");

	//Fork to execute alice2
	pid = fork();

	//child process
	if(pid==0)
	{
		//child process alice2
		static char *argv[]={"alice2",NULL};
		execv("alice2", argv);
		exit(-1);
	}

	//Waits for alice2 to finish
	wait(NULL);

	//Send encrypted messages generated by alice2 to Bob
	enc_mes = fopen("enc_messages.txt", "r");
	fscanf(enc_mes, "%s",buffer);
	strcat(buffer,"\n");
	send(bob, buffer, strlen(buffer), 0);    //A10

	fscanf(enc_mes,"%s",buffer);
	send(bob, buffer, strlen(buffer), 0);    //A11

	fclose(enc_mes);
	printf("Encrypted messages sent to Bob\n");

	//Close connection with Bob
	close(bob);
	printf("Transfer Completed\n");	

	free(l1);
	free(l2);
	free(l3);
	free(l4);
	free(e1);
	free(e2);
	free(e3);
	free(e4);
}