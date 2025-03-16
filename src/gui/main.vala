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

    public static int main(string[] args) {
        return new GTKSolverApp().run(args);
    }
}
