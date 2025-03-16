using Gtk;

[GtkTemplate (ui = "/org/gtksolver/trainer/trainer_window.ui")]
public class TrainerWindow : ApplicationWindow {
    [GtkChild]
    private unowned Button load_corpus_button;
    
    [GtkChild]
    private unowned Button select_output_button;
    
    [GtkChild]
    private unowned Button start_training_button;
    
    [GtkChild]
    private unowned Button stop_training_button;
    
    [GtkChild]
    private unowned Entry corpus_path_entry;
    
    [GtkChild]
    private unowned Entry output_path_entry;
    
    [GtkChild]
    private unowned SpinButton epochs_spin;
    
    [GtkChild]
    private unowned SpinButton batch_size_spin;
    
    [GtkChild]
    private unowned SpinButton learning_rate_spin;
    
    [GtkChild]
    private unowned ProgressBar training_progress;
    
    [GtkChild]
    private unowned TextView log_text_view;
    
    [GtkChild]
    private unowned Label status_label;
    
    [GtkChild]
    private unowned HeaderBar header_bar;
    
    [GtkChild]
    private unowned Spinner training_spinner;
    
    private TrainingManager training_manager;
    private TextBuffer log_buffer;
    
    public TrainerWindow(Gtk.Application app) {
        Object(application: app);
        
        // Initialize managers
        training_manager = new TrainingManager();
        log_buffer = log_text_view.buffer;
        
        // Connect signals
        load_corpus_button.clicked.connect(on_load_corpus_clicked);
        select_output_button.clicked.connect(on_select_output_clicked);
        start_training_button.clicked.connect(on_start_training_clicked);
        stop_training_button.clicked.connect(on_stop_training_clicked);
        
        // Initial state setup
        stop_training_button.sensitive = false;
        start_training_button.sensitive = false;
        
        // Set window title
        header_bar.title_widget = new Gtk.Label("GTKSolver - Обучение модели");
        
        // Setup progress reporting
        training_manager.progress_changed.connect((progress, message) => {
            training_progress.fraction = progress;
            status_label.label = message;
            
            // Add message to log
            append_to_log(message);
        });
        
        training_manager.training_completed.connect((success, message) => {
            training_spinner.stop();
            stop_training_button.sensitive = false;
            start_training_button.sensitive = true;
            
            if (success) {
                show_info_dialog("Обучение завершено", message);
            } else {
                show_error_dialog("Ошибка обучения", message);
            }
            
            append_to_log(message);
        });
    }
    
    private void append_to_log(string message) {
        TextIter end;
        log_buffer.get_end_iter(out end);
        log_buffer.insert(ref end, message + "\n", -1);
        
        // Auto-scroll to the end
        TextIter end_for_scroll;
        log_buffer.get_end_iter(out end_for_scroll);
        log_text_view.scroll_to_iter(end_for_scroll, 0, false, 0, 0);
    }
    
    private void on_load_corpus_clicked() {
        var file_chooser = new FileChooserDialog(
            "Загрузить корпус текстов", this, FileChooserAction.OPEN,
            "_Отмена", ResponseType.CANCEL,
            "_Открыть", ResponseType.ACCEPT
        );
        
        var filter = new FileFilter();
        filter.add_mime_type("text/plain");
        filter.set_name("Текстовые файлы");
        file_chooser.add_filter(filter);
        
        file_chooser.response.connect((dialog, response) => {
            if (response == ResponseType.ACCEPT) {
                corpus_path_entry.text = file_chooser.get_file().get_path();
                update_start_button_state();
            }
            file_chooser.destroy();
        });
        
        file_chooser.present();
    }
    
    private void on_select_output_clicked() {
        var file_chooser = new FileChooserDialog(
            "Выберите файл для сохранения модели", this, FileChooserAction.SAVE,
            "_Отмена", ResponseType.CANCEL,
            "_Сохранить", ResponseType.ACCEPT
        );
        
        file_chooser.set_current_name("model.pt");
        
        // Фильтр для файлов PyTorch
        var filter = new FileFilter();
        filter.add_pattern("*.pt");
        filter.set_name("Файлы модели PyTorch (*.pt)");
        file_chooser.add_filter(filter);
        
        file_chooser.response.connect((dialog, response) => {
            if (response == ResponseType.ACCEPT) {
                output_path_entry.text = file_chooser.get_file().get_path();
                update_start_button_state();
            }
            file_chooser.destroy();
        });
        
        file_chooser.present();
    }
    
    private void update_start_button_state() {
        start_training_button.sensitive = 
            corpus_path_entry.text.strip() != "" && 
            output_path_entry.text.strip() != "";
    }
    
    private void on_start_training_clicked() {
        // Get parameters
        string corpus_path = corpus_path_entry.text;
        string output_path = output_path_entry.text;
        int epochs = (int)epochs_spin.value;
        int batch_size = (int)batch_size_spin.value;
        double learning_rate = learning_rate_spin.value;
        
        // Clear log and reset progress
        log_buffer.text = "";
        training_progress.fraction = 0;
        
        // Start spinner
        training_spinner.start();
        
        // Update UI state
        start_training_button.sensitive = false;
        stop_training_button.sensitive = true;
        
        // Start training
        training_manager.start_training.begin(
            corpus_path, output_path, epochs, batch_size, learning_rate,
            (obj, res) => {
                try {
                    training_manager.start_training.end(res);
                } catch (Error e) {
                    show_error_dialog("Ошибка", e.message);
                    training_spinner.stop();
                    stop_training_button.sensitive = false;
                    start_training_button.sensitive = true;
                }
            }
        );
    }
    
    private void on_stop_training_clicked() {
        training_manager.stop_training();
        stop_training_button.sensitive = false;
        append_to_log("Остановка обучения...");
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
