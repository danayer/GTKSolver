using Gtk;

[GtkTemplate (ui = "/org/gtksolver/app/window.ui")]
public class GTKSolverWindow : ApplicationWindow {
    [GtkChild]
    private unowned TextView task_text_view;
    
    [GtkChild]
    private unowned TextView solution_text_view;
    
    [GtkChild]
    private unowned Button solve_button;
    
    [GtkChild]
    private unowned Button load_task_button;
    
    [GtkChild]
    private unowned Button load_model_button;
    
    [GtkChild]
    private unowned Button save_solution_button;
    
    [GtkChild]
    private unowned HeaderBar header_bar;
    
    [GtkChild]
    private unowned Spinner loading_spinner;
    
    private ModelManager model_manager;
    private TaskProcessor task_processor;
    
    public GTKSolverWindow(Gtk.Application app) {
        Object(application: app);
        
        // Initialize managers
        model_manager = new ModelManager();
        task_processor = new TaskProcessor();
        
        // Connect signals
        solve_button.clicked.connect(on_solve_clicked);
        load_task_button.clicked.connect(on_load_task_clicked);
        load_model_button.clicked.connect(on_load_model_clicked);
        save_solution_button.clicked.connect(on_save_solution_clicked);
        
        // Initial state setup
        save_solution_button.sensitive = false;
        solve_button.sensitive = false;
        
        // Set window title
        header_bar.title_widget = new Gtk.Label("GTKSolver");
    }
    
    private void on_solve_clicked() {
        loading_spinner.start();
        
        var task_buffer = task_text_view.buffer;
        var task_text = task_buffer.text;
        
        if (task_text.strip() == "") {
            show_error_dialog("Ошибка", "Введите текст задачи");
            loading_spinner.stop();
            return;
        }
        
        if (!model_manager.is_model_loaded()) {
            show_error_dialog("Ошибка", "Сначала загрузите модель");
            loading_spinner.stop();
            return;
        }
        
        task_processor.process_task.begin(task_text, model_manager.get_model(), (obj, res) => {
            try {
                string solution = task_processor.process_task.end(res);
                solution_text_view.buffer.text = solution;
                save_solution_button.sensitive = true;
                loading_spinner.stop();
            } catch (Error e) {
                show_error_dialog("Ошибка при решении", e.message);
                loading_spinner.stop();
            }
        });
    }
    
    private void on_load_task_clicked() {
        var file_chooser = new FileChooserDialog(
            "Загрузить задачу", this, FileChooserAction.OPEN,
            "_Отмена", ResponseType.CANCEL,
            "_Открыть", ResponseType.ACCEPT
        );
        
        var filter = new FileFilter();
        filter.add_mime_type("text/plain");
        filter.set_name("Текстовые файлы");
        file_chooser.add_filter(filter);
        
        file_chooser.response.connect((dialog, response) => {
            if (response == ResponseType.ACCEPT) {
                try {
                    string content;
                    File file = File.new_for_path(file_chooser.get_file().get_path());
                    FileUtils.get_contents(file.get_path(), out content);
                    task_text_view.buffer.text = content;
                    solve_button.sensitive = true;
                } catch (Error e) {
                    show_error_dialog("Ошибка загрузки", e.message);
                }
            }
            file_chooser.destroy();
        });
        
        file_chooser.present();
    }
    
    private void on_load_model_clicked() {
        var file_chooser = new FileChooserDialog(
            "Загрузить модель", this, FileChooserAction.OPEN,
            "_Отмена", ResponseType.CANCEL,
            "_Открыть", ResponseType.ACCEPT
        );
        
        var filter = new FileFilter();
        filter.add_pattern("*.pt");
        filter.set_name("Файлы модели PyTorch (*.pt)");
        file_chooser.add_filter(filter);
        
        file_chooser.response.connect((dialog, response) => {
            if (response == ResponseType.ACCEPT) {
                loading_spinner.start();
                model_manager.load_model.begin(file_chooser.get_file().get_path(), (obj, res) => {
                    try {
                        model_manager.load_model.end(res);
                        solve_button.sensitive = true;
                        show_info_dialog("Модель загружена", "Модель успешно загружена");
                    } catch (Error e) {
                        show_error_dialog("Ошибка загрузки модели", e.message);
                    }
                    loading_spinner.stop();
                });
            }
            file_chooser.destroy();
        });
        
        file_chooser.present();
    }
    
    private void on_save_solution_clicked() {
        var file_chooser = new FileChooserDialog(
            "Сохранить решение", this, FileChooserAction.SAVE,
            "_Отмена", ResponseType.CANCEL,
            "_Сохранить", ResponseType.ACCEPT
        );
        
        file_chooser.set_current_name("решение.txt");
        
        file_chooser.response.connect((dialog, response) => {
            if (response == ResponseType.ACCEPT) {
                try {
                    string content = solution_text_view.buffer.text;
                    FileUtils.set_contents(file_chooser.get_file().get_path(), content);
                    show_info_dialog("Сохранено", "Решение успешно сохранено");
                } catch (Error e) {
                    show_error_dialog("Ошибка сохранения", e.message);
                }
            }
            file_chooser.destroy();
        });
        
        file_chooser.present();
    }
    
    private void show_error_dialog(string title, string message) {
        var dialog = new MessageDialog(
            this,
            DialogFlags.MODAL,
            MessageType.ERROR,
            ButtonsType.OK,
            "%s", message
        );
        dialog.set_title(title);
        dialog.response.connect((dialog, response) => {
            dialog.destroy();
        });
        dialog.present();
    }
    
    private void show_info_dialog(string title, string message) {
        var dialog = new MessageDialog(
            this,
            DialogFlags.MODAL,
            MessageType.INFO,
            ButtonsType.OK,
            "%s", message
        );
        dialog.set_title(title);
        dialog.response.connect((dialog, response) => {
            dialog.destroy();
        });
        dialog.present();
    }
}
