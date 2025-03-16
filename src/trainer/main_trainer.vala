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

    public static int main(string[] args) {
        return new GTKSolverTrainer().run(args);
    }
}
