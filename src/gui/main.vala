using Gtk;

public class GTKSolverApp : Gtk.Application {
    public GTKSolverApp() {
        Object(application_id: "org.gtksolver.app", 
               flags: ApplicationFlags.FLAGS_NONE);
    }

    protected override void activate() {
        var win = new GTKSolverWindow(this);
        win.present();
    }
}

// Export function for C to call
[CCode (cname = "gtk_solver_app_main")]
public void gtk_solver_app_main(string[] args) {
    var app = new GTKSolverApp();
    app.run(args);
}
