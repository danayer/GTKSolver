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
        var file_dialog = new FileDialog();
        file_dialog.set_title("Загрузить задачу");
        
        var filter = new FileFilter();
        filter.add_mime_type("text/plain");
        // Use add_pattern instead of set_pattern
        filter.add_pattern("*.txt");
        
        // Use explicit property assignment for name
        filter.name = "Текстовые файлы";
        
        // Use fully qualified name for ListStore
        var filters = new GLib.ListStore(typeof(FileFilter));
        filters.append(filter);
        file_dialog.set_filters(filters);
        
        file_dialog.open.begin(this, null, (obj, res) => {
            try {
                var file = file_dialog.open.end(res);
                if (file != null) {
                    string content;
                    FileUtils.get_contents(file.get_path(), out content);
                    task_text_view.buffer.text = content;
                    solve_button.sensitive = true;
                }
            } catch (Error e) {
                show_error_dialog("Ошибка загрузки", e.message);
            }
        });
    }
    
    private void on_load_model_clicked() {
        var file_dialog = new FileDialog();
        file_dialog.set_title("Загрузить модель");
        
        var filter = new FileFilter();
        // Use add_pattern instead of set_pattern
        filter.add_pattern("*.pt");
        filter.name = "Файлы модели PyTorch (*.pt)";
        
        // Use fully qualified name for ListStore
        var filters = new GLib.ListStore(typeof(FileFilter));
        filters.append(filter);
        file_dialog.set_filters(filters);
        
        file_dialog.open.begin(this, null, (obj, res) => {
            try {
                var file = file_dialog.open.end(res);
                if (file != null) {
                    loading_spinner.start();
                    model_manager.load_model.begin(file.get_path(), (obj, res) => {
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
            } catch (Error e) {
                show_error_dialog("Ошибка выбора файла", e.message);
            }
        });
    }
    
    private void on_save_solution_clicked() {
        var file_dialog = new FileDialog();
        file_dialog.set_title("Сохранить решение");
        file_dialog.set_initial_name("решение.txt");
        
        file_dialog.save.begin(this, null, (obj, res) => {
            try {
                var file = file_dialog.save.end(res);
                if (file != null) {
                    string content = solution_text_view.buffer.text;
                    FileUtils.set_contents(file.get_path(), content);
                    show_info_dialog("Сохранено", "Решение успешно сохранено");
                }
            } catch (Error e) {
                show_error_dialog("Ошибка сохранения", e.message);
            }
        });
    }
    
    private void show_error_dialog(string title, string message) {
        // Create AlertDialog with required parameters
        var dialog = new AlertDialog(title, message);
        dialog.set_buttons({"OK"});
        dialog.set_modal(true);
        dialog.show(this);
    }
    
    private void show_info_dialog(string title, string message) {
        // Create AlertDialog with required parameters
        var dialog = new AlertDialog(title, message);
        dialog.set_buttons({"OK"});
        dialog.set_modal(true);
        dialog.show(this);
    }
}
