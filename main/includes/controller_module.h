#ifndef CONTROLLER_MODULE_H_INCLUDDED
#define CONTROLLER_MODULE_H_INCLUDDED

typedef void (*button_cb)();

void controllerInit();
void controllerRegisterButtonUp(button_cb cb);
void controllerRegisterButtonSelect(button_cb cb);
void controllerRegisterButtonDown(button_cb cb);

#endif