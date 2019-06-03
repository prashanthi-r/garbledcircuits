#include <stdio.h> 
#include <stdlib.h>
#include <gmp.h>
#include "rsa.h"

int main(){
	//Open required files
	FILE *randomx, *public, *private_b, *enc_messages *message;
	//randomx = fopen("randomx.txt", "r"); //file to store randomly generated x_0, x_1
	//public = fopen("public_key.txt", "r"); //file containing Alice's public key
	c_out = fopen("bob_c.txt", "w"); //stores c = (x_b + k^e) mod n
	private_b = fopen("bob_private.txt","w"); //File contains random k generated in bob_ot1.c
	enc_messages = fopen("enc_messages.txt", "r"); // to write m'_0 (= m_0 - v_0) and m'_1 (= m_1 - v_1) to "enc_messages.txt"
	message = fopen("bob_message.txt","w");
	
	//Variable initialisation
	mpz_t c,m_b,k,m_0,m_1;
	int b;
	mpz_init(m_b);
	mpz_init(m_1);
	mpz_init(m_0);
	mpz_init(c);
	mpz_init(k);

	//Reading k generated in bob_ot1.c
	gmp_fscanf(private_b, "%Zd", k);
	
	//Reading n from public_key.txt
	//gmp_fscanf(public, "%Zd", n);

	//reading x_0 and x_1 from randomx.txt
	gmp_fscanf(enc_messages, "%Zd", m_0);
	gmp_fscanf(enc_messages, "%Zd", m_1);

	//closing public_key.txt and randomx.txt
	fclose(public);
	fclose(enc_messages);

	//Reading b chosen in bob_ot1.c
	fscanf(private_b,"%d",&b);

	//Initialising x_b based on b
	if(b==1)
		mpz_set(m_b,m_1);
	else
		mpz_set(m_b,m_0);
	
	mpz_clear(m_0);
	mpz_clear(m_1);

	//calculating m_b=m_b-k
	mpz_sub(m_b,m_b,k);

	//Writing c to bob_c.txt
	mpz_out_str(message,10,m_b);

	fclose(message);
	mpz_clear(k);
	mpz_clear(c);
	mpz_clear(m_b);
}