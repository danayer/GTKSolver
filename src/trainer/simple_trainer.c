#include <gtk/gtk.h>
#include <stdio.h>

// Global UI elements
static GtkWidget *corpus_path_entry;
static GtkWidget *output_path_entry;
static GtkWidget *epochs_spin;
static GtkWidget *batch_size_spin;
static GtkWidget *learning_rate_spin;
static GtkWidget *training_progress;
static GtkWidget *log_text_view;
static GtkWidget *status_label;
static GtkWidget *start_training_button;
static GtkWidget *stop_training_button;
static GtkWidget *training_spinner;

// Forward declarations
static void on_load_corpus_clicked(GtkButton *button, gpointer user_data);
static void on_select_output_clicked(GtkButton *button, gpointer user_data);
static void on_start_training_clicked(GtkButton *button, gpointer user_data);
static void on_stop_training_clicked(GtkButton *button, gpointer user_data);
static void update_start_button_state(void);
static void show_dialog(GtkWindow *parent, const char *title, const char *message, GtkMessageType type);
static void append_to_log(const char *message);

// Add callbacks forward declarations
static void file_open_callback(GObject *source, GAsyncResult *result, gpointer user_data);
static void file_save_callback(GObject *source, GAsyncResult *result, gpointer user_data);

// Mock training state
static gboolean is_training = FALSE;
static gdouble training_progress_value = 0.0;
static guint training_timer_id = 0;

// Add these global variables to track simulation progress
static int total_epochs = 50;  // Default
static int current_epoch = 0;
static const char* current_corpus_path = NULL;
static const char* current_output_path = NULL;

static GtkWidget* create_trainer_ui(GtkApplication *app) {
    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GTKSolver Trainer");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 700);
    
    // Create the main vertical box container
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(main_box, 12);
    gtk_widget_set_margin_end(main_box, 12);
    gtk_widget_set_margin_top(main_box, 12);
    gtk_widget_set_margin_bottom(main_box, 12);
    
    // Create a header label
    GtkWidget *header_label = gtk_label_new("Настройка обучения модели");
    gtk_widget_set_halign(header_label, GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(header_label, 12);
    // Make this header bold and larger
    PangoAttrList *attr_list = pango_attr_list_new();
    pango_attr_list_insert(attr_list, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    pango_attr_list_insert(attr_list, pango_attr_scale_new(1.2));
    gtk_label_set_attributes(GTK_LABEL(header_label), attr_list);
    pango_attr_list_unref(attr_list);
    
    gtk_box_append(GTK_BOX(main_box), header_label);
    
    // Create a grid for settings
    GtkWidget *settings_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(settings_grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(settings_grid), 12);
    gtk_widget_set_hexpand(settings_grid, TRUE);
    
    // Corpus path row
    GtkWidget *corpus_label = gtk_label_new("Корпус текстов:");
    gtk_widget_set_halign(corpus_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(settings_grid), corpus_label, 0, 0, 1, 1);
    
    GtkWidget *corpus_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    corpus_path_entry = gtk_entry_new();
    gtk_widget_set_hexpand(corpus_path_entry, TRUE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(corpus_path_entry), "Путь к файлу корпуса");
    
    GtkWidget *load_corpus_button = gtk_button_new_with_label("Выбрать");
    g_signal_connect(load_corpus_button, "clicked", G_CALLBACK(on_load_corpus_clicked), window);
    
    gtk_box_append(GTK_BOX(corpus_box), corpus_path_entry);
    gtk_box_append(GTK_BOX(corpus_box), load_corpus_button);
    gtk_grid_attach(GTK_GRID(settings_grid), corpus_box, 1, 0, 1, 1);
    
    // Output path row
    GtkWidget *output_label = gtk_label_new("Выходной файл:");
    gtk_widget_set_halign(output_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(settings_grid), output_label, 0, 1, 1, 1);
    
    GtkWidget *output_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    output_path_entry = gtk_entry_new();
    gtk_widget_set_hexpand(output_path_entry, TRUE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(output_path_entry), "Путь для сохранения модели");
    
    GtkWidget *select_output_button = gtk_button_new_with_label("Выбрать");
    g_signal_connect(select_output_button, "clicked", G_CALLBACK(on_select_output_clicked), window);
    
    gtk_box_append(GTK_BOX(output_box), output_path_entry);
    gtk_box_append(GTK_BOX(output_box), select_output_button);
    gtk_grid_attach(GTK_GRID(settings_grid), output_box, 1, 1, 1, 1);
    
    // Epochs row
    GtkWidget *epochs_label = gtk_label_new("Количество эпох:");
    gtk_widget_set_halign(epochs_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(settings_grid), epochs_label, 0, 2, 1, 1);
    
    GtkAdjustment *epochs_adj = gtk_adjustment_new(50, 1, 1000, 1, 10, 0);
    epochs_spin = gtk_spin_button_new(epochs_adj, 1, 0);
    gtk_grid_attach(GTK_GRID(settings_grid), epochs_spin, 1, 2, 1, 1);
    
    // Batch size row
    GtkWidget *batch_label = gtk_label_new("Размер батча:");
    gtk_widget_set_halign(batch_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(settings_grid), batch_label, 0, 3, 1, 1);
    
    GtkAdjustment *batch_adj = gtk_adjustment_new(32, 1, 256, 1, 8, 0);
    batch_size_spin = gtk_spin_button_new(batch_adj, 1, 0);
    gtk_grid_attach(GTK_GRID(settings_grid), batch_size_spin, 1, 3, 1, 1);
    
    // Learning rate row
    GtkWidget *lr_label = gtk_label_new("Скорость обучения:");
    gtk_widget_set_halign(lr_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(settings_grid), lr_label, 0, 4, 1, 1);
    
    GtkAdjustment *lr_adj = gtk_adjustment_new(0.001, 0.0001, 0.1, 0.0001, 0.001, 0);
    learning_rate_spin = gtk_spin_button_new(lr_adj, 0.0001, 4);
    gtk_grid_attach(GTK_GRID(settings_grid), learning_rate_spin, 1, 4, 1, 1);
    
    gtk_box_append(GTK_BOX(main_box), settings_grid);
    
    // Control buttons
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_top(button_box, 12);
    gtk_widget_set_margin_bottom(button_box, 12);
    
    start_training_button = gtk_button_new_with_label("Начать обучение");
    gtk_widget_add_css_class(start_training_button, "suggested-action");
    gtk_widget_set_hexpand(start_training_button, TRUE);
    g_signal_connect(start_training_button, "clicked", G_CALLBACK(on_start_training_clicked), window);
    
    stop_training_button = gtk_button_new_with_label("Остановить");
    gtk_widget_add_css_class(stop_training_button, "destructive-action");
    gtk_widget_set_hexpand(stop_training_button, TRUE);
    gtk_widget_set_sensitive(stop_training_button, FALSE);
    g_signal_connect(stop_training_button, "clicked", G_CALLBACK(on_stop_training_clicked), window);
    
    gtk_box_append(GTK_BOX(button_box), start_training_button);
    gtk_box_append(GTK_BOX(button_box), stop_training_button);
    gtk_box_append(GTK_BOX(main_box), button_box);
    
    // Progress label
    GtkWidget *progress_label = gtk_label_new("Прогресс обучения:");
    gtk_widget_set_halign(progress_label, GTK_ALIGN_START);
    gtk_widget_set_margin_top(progress_label, 6);
    gtk_box_append(GTK_BOX(main_box), progress_label);
    
    // Progress bar
    training_progress = gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(training_progress), TRUE);
    gtk_box_append(GTK_BOX(main_box), training_progress);
    
    // Status label
    status_label = gtk_label_new("Готов к обучению");
    gtk_widget_set_halign(status_label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(status_label, 6);
    gtk_widget_set_margin_bottom(status_label, 12);
    gtk_box_append(GTK_BOX(main_box), status_label);
    
    // Log label
    GtkWidget *log_label = gtk_label_new("Журнал обучения:");
    gtk_widget_set_halign(log_label, GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(log_label, 6);
    gtk_box_append(GTK_BOX(main_box), log_label);
    
    // Log view in a scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 200);
    
    log_text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(log_text_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(log_text_view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(log_text_view), GTK_WRAP_WORD);
    gtk_widget_set_margin_bottom(log_text_view, 12);
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), log_text_view);
    gtk_box_append(GTK_BOX(main_box), scrolled_window);
    
    // Create spinner in header
    GtkWidget *header_bar = gtk_header_bar_new();
    training_spinner = gtk_spinner_new();
    gtk_widget_set_visible(training_spinner, TRUE);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), training_spinner);
    
    // Set up window
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);
    gtk_window_set_child(GTK_WINDOW(window), main_box);
    
    // Initial state
    gtk_widget_set_sensitive(start_training_button, FALSE);
    
    // Connect entry change signals for updating button state
    g_signal_connect_swapped(corpus_path_entry, "changed", G_CALLBACK(update_start_button_state), NULL);
    g_signal_connect_swapped(output_path_entry, "changed", G_CALLBACK(update_start_button_state), NULL);
    
    return window;
}

// Event handlers
static void on_load_corpus_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Загрузить корпус текстов");
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_mime_type(filter, "text/plain");
    gtk_file_filter_set_name(filter, "Текстовые файлы");
    
    GListStore *filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(filters, filter);
    gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(filters));
    
    gtk_file_dialog_open(dialog, window, NULL, (GAsyncReadyCallback)file_open_callback, corpus_path_entry);
}

static void file_open_callback(GObject *source, GAsyncResult *result, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GtkEntry *entry = GTK_ENTRY(user_data);
    GFile *file;
    
    file = gtk_file_dialog_open_finish(dialog, result, NULL);
    if (file) {
        char *path = g_file_get_path(file);
        gtk_editable_set_text(GTK_EDITABLE(entry), path);  // Use gtk_editable_set_text instead
        g_free(path);
        g_object_unref(file);
    }
    
    g_object_unref(dialog);
}

static void on_select_output_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Выберите файл для сохранения модели");
    gtk_file_dialog_set_initial_name(dialog, "model.pt");
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.pt");
    gtk_file_filter_set_name(filter, "Файлы модели PyTorch (*.pt)");
    
    GListStore *filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(filters, filter);
    gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(filters));
    
    gtk_file_dialog_save(dialog, window, NULL, (GAsyncReadyCallback)file_save_callback, output_path_entry);
}

static void file_save_callback(GObject *source, GAsyncResult *result, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GtkEntry *entry = GTK_ENTRY(user_data);
    GFile *file;
    
    file = gtk_file_dialog_save_finish(dialog, result, NULL);
    if (file) {
        char *path = g_file_get_path(file);
        gtk_editable_set_text(GTK_EDITABLE(entry), path);  // Use gtk_editable_set_text instead
        g_free(path);
        g_object_unref(file);
    }
    
    g_object_unref(dialog);
}

static void update_start_button_state(void) {
    const char *corpus_text = gtk_editable_get_text(GTK_EDITABLE(corpus_path_entry));  // Use gtk_editable_get_text
    const char *output_text = gtk_editable_get_text(GTK_EDITABLE(output_path_entry));  // Use gtk_editable_get_text
    
    gboolean can_start = (corpus_text && *corpus_text) && (output_text && *output_text);
    gtk_widget_set_sensitive(start_training_button, can_start);
}

static void append_to_log(const char *message) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(log_text_view));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    
    char *full_message = g_strdup_printf("%s\n", message);
    gtk_text_buffer_insert(buffer, &end, full_message, -1);
    g_free(full_message);
    
    // Scroll to end
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(log_text_view),
                               gtk_text_buffer_get_insert(buffer), 0.0, FALSE, 0, 0);
}

static gboolean update_training_progress(gpointer user_data) {
    if (!is_training) {
        return G_SOURCE_REMOVE;
    }
    
    // Simulate training progress based on current/total epochs
    training_progress_value += 0.01;
    if (training_progress_value > 1.0) {
        // One epoch completed
        current_epoch++;
        training_progress_value = 0.0;
        
        // Check if we've reached the total epochs
        if (current_epoch >= total_epochs) {
            // Training completed, save the model and stop
            char completion_message[256];
            snprintf(completion_message, sizeof(completion_message),
                    "Обучение завершено. Модель сохранена в %s", current_output_path);
            append_to_log(completion_message);
            gtk_label_set_text(GTK_LABEL(status_label), "Обучение завершено");
            
            // Write an empty file to simulate model saving
            FILE* output_file = fopen(current_output_path, "w");
            if (output_file) {
                fprintf(output_file, "# Mock model file created by GTKSolver Trainer\n");
                fprintf(output_file, "# Parameters:\n");
                fprintf(output_file, "# - Epochs: %d\n", total_epochs);
                fprintf(output_file, "# - Corpus: %s\n", current_corpus_path);
                fclose(output_file);
                
                char saved_message[256];
                snprintf(saved_message, sizeof(saved_message),
                        "Модель успешно сохранена в %s", current_output_path);
                append_to_log(saved_message);
            } else {
                append_to_log("Ошибка при сохранении модели");
            }
            
            // Stop the training
            is_training = FALSE;
            if (training_timer_id > 0) {
                g_source_remove(training_timer_id);
                training_timer_id = 0;
            }
            
            // Update UI
            gtk_spinner_stop(GTK_SPINNER(training_spinner));
            gtk_widget_set_sensitive(stop_training_button, FALSE);
            gtk_widget_set_sensitive(start_training_button, TRUE);
            
            return G_SOURCE_REMOVE;
        }
    }
    
    // Update progress bar to show overall progress across all epochs
    double overall_progress = (current_epoch + training_progress_value) / total_epochs;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(training_progress), overall_progress);
    
    char progress_text[32];
    snprintf(progress_text, sizeof(progress_text), "Эпоха %d/%d: %.0f%%", 
             current_epoch + 1, total_epochs, training_progress_value * 100);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(training_progress), progress_text);
    
    // Simulate log messages occasionally
    if (g_random_int_range(0, 20) == 0) {
        char log_message[256];
        snprintf(log_message, sizeof(log_message), 
                "Эпоха %d/%d: Потери: %.4f, Точность: %.2f%%", 
                current_epoch + 1, total_epochs,
                1.0 - training_progress_value * 0.8, 
                training_progress_value * 90.0);
        append_to_log(log_message);
        
        // Update status label
        gtk_label_set_text(GTK_LABEL(status_label), log_message);
    }
    
    return G_SOURCE_CONTINUE;
}

static void on_start_training_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    
    // Get parameters
    current_corpus_path = gtk_editable_get_text(GTK_EDITABLE(corpus_path_entry));
    current_output_path = gtk_editable_get_text(GTK_EDITABLE(output_path_entry));
    total_epochs = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(epochs_spin));
    int batch_size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(batch_size_spin));
    double learning_rate = gtk_spin_button_get_value(GTK_SPIN_BUTTON(learning_rate_spin));
    
    // Reset epoch counter
    current_epoch = 0;
    
    // Clear log and reset progress
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(log_text_view));
    gtk_text_buffer_set_text(buffer, "", -1);
    training_progress_value = 0.0;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(training_progress), 0.0);
    
    // Start spinner
    gtk_spinner_start(GTK_SPINNER(training_spinner));
    
    // Update UI state
    gtk_widget_set_sensitive(start_training_button, FALSE);
    gtk_widget_set_sensitive(stop_training_button, TRUE);
    
    // Start training (simulated)
    is_training = TRUE;
    
    // Log parameters
    char message[512];
    snprintf(message, sizeof(message), 
            "Начало обучения с параметрами:\nКорпус: %s\nВыходной файл: %s\nЭпох: %d\nРазмер батча: %d\nСкорость обучения: %.4f",
            current_corpus_path, current_output_path, total_epochs, batch_size, learning_rate);
    append_to_log(message);
    gtk_label_set_text(GTK_LABEL(status_label), "Обучение начато");
    
    // Start timer to update progress
    training_timer_id = g_timeout_add(100, update_training_progress, NULL);
}

static void on_stop_training_clicked(GtkButton *button, gpointer user_data) {
    // Stop training
    is_training = FALSE;
    
    // Stop the update timer
    if (training_timer_id > 0) {
        g_source_remove(training_timer_id);
        training_timer_id = 0;
    }
    
    // Stop spinner
    gtk_spinner_stop(GTK_SPINNER(training_spinner));
    
    // Update UI state
    gtk_widget_set_sensitive(stop_training_button, FALSE);
    gtk_widget_set_sensitive(start_training_button, TRUE);
    
    // Save partial model if requested
    if (current_output_path && current_corpus_path) {
        FILE* output_file = fopen(current_output_path, "w");
        if (output_file) {
            fprintf(output_file, "# Partial model file created by GTKSolver Trainer\n");
            fprintf(output_file, "# Parameters:\n");
            fprintf(output_file, "# - Completed epochs: %d/%d\n", current_epoch, total_epochs);
            fprintf(output_file, "# - Corpus: %s\n", current_corpus_path);
            fclose(output_file);
            
            char saved_message[256];
            snprintf(saved_message, sizeof(saved_message),
                    "Частичная модель сохранена в %s", current_output_path);
            append_to_log(saved_message);
        } else {
            append_to_log("Ошибка при сохранении модели");
        }
    }
    
    // Log message
    append_to_log("Обучение остановлено пользователем");
    gtk_label_set_text(GTK_LABEL(status_label), "Обучение остановлено");
}

static void show_dialog(GtkWindow *parent, const char *title, const char *message, GtkMessageType type) {
    GtkAlertDialog *dialog = gtk_alert_dialog_new("%s", message);
    gtk_alert_dialog_set_modal(dialog, TRUE);
    gtk_alert_dialog_set_detail(dialog, title);
    gtk_alert_dialog_show(dialog, parent);
    g_object_unref(dialog);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = create_trainer_ui(app);
    gtk_window_present(GTK_WINDOW(window));
    printf("GTKSolver Trainer window created and presented\n");
}

int main(int argc, char *argv[]) {
    // Create a GTK application
    GtkApplication *app = gtk_application_new("org.gtksolver.trainer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    // Run the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
