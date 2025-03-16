using Gtk;

public class GTKSolverTrainer : Gtk.Application {
    public GTKSolverTrainer() {
        Object(application_id: "org.gtksolver.trainer", 
               flags: ApplicationFlags.FLAGS_NONE);
    }

    protected override void activate() {
        var win = new TrainerWindow(this);
        win.present();
    }
}

// Export function for C to call
[CCode (cname = "gtk_solver_trainer_main")]
public void gtk_solver_trainer_main(string[] args) {
    var app = new GTKSolverTrainer();
    app.run(args);
}
