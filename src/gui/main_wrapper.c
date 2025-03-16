#include <stdio.h>
#include <stdlib.h>

// Declaration of the Vala entry point
extern void gtk_solver_app_main(int argc, char **argv);

int main(int argc, char *argv[]) {
    printf("Starting GTKSolver GUI application\n");
    
    // Call the Vala entry point directly
    gtk_solver_app_main(argc, argv);
    
    return 0;
}
