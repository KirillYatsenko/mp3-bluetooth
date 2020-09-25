/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "controller_module.h"
#include "menu_module.h"

int app_main(void)
{
    menuDisplayMain();

    controllerInit();
    controllerRegisterButtonUp(menuPressUp);
    controllerRegisterButtonSelect(menuPressSelect);
    controllerRegisterButtonDown(menuPressDown);

    return 0;
}