#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../Header/menu.h"

int main(){
    int menu = 0;
    int login = 0;
    
    do{
        //Check option of user
        menu = 0;
        menu = getMenu();

        //Function of menu
        if(menu == 1){
            loginAccount(&login);     
        };
        
        if(menu == 2){
            postMessage(login);
        };
        
        if(menu == 3){
            logoutAccount(&login);
        };
        
        if(menu == 4){
            logActivity("4", "",  "+OK");
        };
        
    }while(menu >=1 && menu < 4);
    
    return 0;
}

