
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "relays.h"
#include "buttons.h"
#include "lcd.h"
#include "uart.h"

enum systState {idle, pick_user, login, signed_in};			//state machine state datatype
enum systState current_state = idle;						//state machines value tracker
bool stateSwitched = false;									//for tracking if state was switched
uint8_t number_of_users = 3;								//number of users existing in the system
uint8_t currentUser = 1;									//currently selected user
uint8_t passwordPosition = 1;								//used for scrolling through the positions of passwords
uint8_t insertedPassword[4] = {0,0,0,0};					//the password inserted by the user
uint8_t userPasswords[12] = {0,0,0,0,0,1,2,3,9,8,7,6};		//the saved passwords for each user
uint8_t wrongPasswordEntries = 0;							//counter for incorrect password entries


int main(void)
{
	//initialization of peripherals
    buttonsInit();
	lcdInit();
	relayInit();
	uartInit();
	
	//main loop
    while (1) 
    {
		//delay for limiting the speed of the system
		_delay_ms(200);
		
		//start with reading button value
		buttonsRead();
		
		//if state was switched, clears the display
		if(stateSwitched){
			lcdClear();
			stateSwitched = false;
		}
		
		//branches the code depending on the current state it is in
		switch(current_state){
			
			//logged out state
			case idle:
			
				//printing the default text for this state
				lcdHome();
				fprintf(&lcd, "Press any button");
				
				//if any button is pressed, move to state pick_user
				if (buttonsGet() != 0){
					current_state = pick_user;
					stateSwitched = true;
				}
				break;
				
			//state for choosing user
			case pick_user:
			
				//printing the default text for this state
				lcdHome();
				fprintf(&lcd, "User %d",currentUser);
				
				//if elseif structure for determining action
				
				//right button, increments the user number
				if((buttonsGet()==5)&&(currentUser<number_of_users)) 
					currentUser++;
				
				//left button, decrements the user number
				else if((buttonsGet()==2)&&(currentUser>1)) 
					currentUser--;
				
				//select button, cancels login, returns to idle state
				else if(buttonsGet()==1){
					current_state = idle;
					stateSwitched = true;
				}
				
				//down button, chooses current user to login as
				else if(buttonsGet()==4){
					current_state = login;
					lcdShowCursor(1);		//shows cursor for password entry assistance
					stateSwitched = true;
					fprintf(&uart,"User %d trying to log in.\n", currentUser);	//sends info by serial comm
				}
				break;
					
			//state requesting password entry, checking password validity
			case login:
				
				//printing the default text for this state
				lcdHome();
				fprintf(&lcd, "User %d",currentUser);
				lcdSetCursor(41);
				fprintf(&lcd,"Pswd: %d%d%d%d",insertedPassword[0],insertedPassword[1],insertedPassword[2],insertedPassword[3]);		//displays the currently entered password
				fprintf(&lcd,"   ");	
				
				//if any wrong password entries occurred, print X up to 3
				for(uint8_t j = wrongPasswordEntries;j>0;j--) 
					fprintf(&lcd,"X");
				
				//sets the cursor to current position in password entry
				lcdSetCursor(47 + passwordPosition - 1);
				
				//left button, for password confirmation
				if(buttonsGet()==2){
					
					//resets var for password validity checking
					bool passwordCorrect = true;
					
					//cycles through inserted password, compares to saved password
					for(int i = 0;i<=3;i++){
						
						//if on a given position the passwords are not equal
						if(insertedPassword[i] != userPasswords[(currentUser-1)*4 + i]){
							passwordCorrect = false;	//password incorrect
							wrongPasswordEntries++;		//incrementing the invalid password tracker
							fprintf(&uart,"Incorrect password entry(%d).\n", wrongPasswordEntries);		//sends info by serial comm
							
							//activates alarm if 3 wrong entries happened
							if(wrongPasswordEntries==3){
								relayControl(2,1);
								fprintf(&uart,"Alarm activated.\n");		//sends info by serial comm
							}
							break;
						}
					}
					
					//if after cycling through passwords the inserted password is correct
					if(passwordCorrect){
						
						//if alarm was activated, deactivate 
						if(relayStatus(2)){
							relayControl(2,0);
							fprintf(&uart,"Alarm deactivated.\n");		//sends info by serial comm
						}
						
						//transition to logged in state, 
						current_state = signed_in;
						stateSwitched = true;
						fprintf(&uart,"Logged in as User %d.\n", currentUser);		//sends info by serial comm
						
						//reset of variables for next login
						wrongPasswordEntries = 0;
						insertedPassword[0] = 0;
						insertedPassword[1] = 0;
						insertedPassword[2] = 0;
						insertedPassword[3] = 0;
						passwordPosition = 1;
						lcdShowCursor(0);		//password entry complete, cursor off
					}
				}
				
				//up button, increments the password value on current position
				else if((buttonsGet()==3) &&(insertedPassword[passwordPosition-1]<9)) 
					insertedPassword[passwordPosition-1]++;
				
				//down button, decrements the password value on current position
				else if((buttonsGet()==4) &&(insertedPassword[passwordPosition-1]>0)) 
					insertedPassword[passwordPosition-1]--;
				
				//left button, for scrolling through the positions in inserted password
				else if(buttonsGet()==5){
					if(passwordPosition==4) passwordPosition = 1;
					else passwordPosition++;
				}
				
				//login cancel
				else if(buttonsGet()==1){
					current_state = idle;
					lcdShowCursor(0);		//cursor off
					stateSwitched = true;
					fprintf(&uart,"Login cancelled.\n");	//sends info by serial comm
				}
				break;
				
				
			//signed in state, control of relays	
			case signed_in:
			
				//printing the default text for this state
				lcdHome();
				fprintf(&lcd, "Lock:      ");
				//displaying the status of lock
				if(relayStatus(1)) fprintf(&lcd, "ON ");
				else fprintf(&lcd, "OFF");
				
				lcdSetCursor(41);
				fprintf(&lcd, "Alarm:     ");
				//displaying the status of alarm
				if(relayStatus(2)) fprintf(&lcd, "ON ");
				else fprintf(&lcd, "OFF");
				
				//up button, toggling the state of lock
				if(buttonsGet() == 3){
					relayControl(1,!relayStatus(1));
					relayStatus(1)? fprintf(&uart,"Lock activated.\n") : fprintf(&uart,"Lock deactivated.\n");		//sends info by serial comm
				}
				
				//down button, toggling alarm
				else if(buttonsGet() == 4){
					relayControl(2,!relayStatus(2));
					relayStatus(2)? fprintf(&uart,"Alarm activated.\n") : fprintf(&uart,"Alarm deactivated.\n");	//sends info by serial comm
				}
				
				//select button, logout
				else if(buttonsGet() == 1){
					current_state = idle;
					stateSwitched = true;
					fprintf(&uart,"Logged out.\n");		//sends info by serial comm
				}
				break;
				
				
			default:
				break;
		}
    }
}

