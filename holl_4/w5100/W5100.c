

void w5100(void)
{
//*********Socket initialization**
	
	start:

	/* sets TCP mode */ 
	Sn_MR = 0x01;   

	/* sets source port number */ 
	Sn_PORT = source_port; 

	/* sets OPEN command */ 
	Sn_CR = OPEN; 

	if (Sn_SR != SOCK_INIT)
	{
		Sn_CR = CLOSE; 
		goto start;
	}	
	
//*********LISTEN*****************
	
	/* listen socket */ 
	Sn_CR = LISTEN; 
	if (Sn_SR != SOCK_LISTEN)
	{
		Sn_CR = CLOSE;
		goto start; // check socket status 
	}

//********* ESTABLISHED ? ********

	
}