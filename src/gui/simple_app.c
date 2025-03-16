#include <gtk/gtk.h>
#include <stdio.h>

// Global UI elements
static GtkWidget *task_text_view;
static GtkWidget *solution_text_view;
static GtkWidget *solve_button;
static GtkWidget *load_task_button;
static GtkWidget *load_model_button;
static GtkWidget *save_solution_button;
static GtkWidget *loading_spinner;

// Forward declarations
static void on_solve_clicked(GtkButton *button, gpointer user_data);
static void on_load_task_clicked(GtkButton *button, gpointer user_data);
static void on_load_model_clicked(GtkButton *button, gpointer user_data);
static void on_save_solution_clicked(GtkButton *button, gpointer user_data);
static void show_dialog(GtkWindow *parent, const char *title, const char *message, GtkMessageType type);

// Callback declarations
static void file_open_callback(GObject *source, GAsyncResult *result, gpointer user_data);
static void model_file_open_callback(GObject *source, GAsyncResult *result, gpointer user_data);
static void file_save_callback(GObject *source, GAsyncResult *result, gpointer user_data);

// Need to declare callback functions for timeouts
static gboolean on_model_loaded_timeout(gpointer user_data);
static gboolean on_solve_completed_timeout(gpointer user_data);

// Structure to pass data to timeout callbacks
typedef struct {
    GtkWindow *window;
    char *path;
    char *task_text;
} TimeoutData;

// Add model "capabilities" structure
typedef struct {
    char name[64];
    char version[32];
    int vocabulary_size;
    int embedding_dim;
    int hidden_dim;
    char specialization[128];
    float accuracy;
} ModelCapabilities;

// Mock model state
static gboolean model_loaded = FALSE;
static char model_path[1024] = {0};
static ModelCapabilities current_model = {0};

// Mock model "database" - different models have different capabilities
static const ModelCapabilities available_models[] = {
    {"basic_math", "1.0", 10000, 256, 512, "basic arithmetic and algebra", 0.82},
    {"calculus", "2.1", 15000, 384, 768, "calculus and differential equations", 0.91},
    {"statistics", "1.5", 12000, 256, 512, "probability and statistics", 0.87},
    {"geometry", "1.8", 8000, 256, 384, "plane and solid geometry", 0.89},
    {"physics", "0.9", 20000, 512, 1024, "mechanics and electromagnetism", 0.78}
};

// Add more sophisticated response generation
static char* generate_varied_response(const char* input) {
    // Analyze input text to determine type of problem
    char* problem_type = "unknown";
    char* approach = "analytical solving";
    char* formula = "f(x) = ?";
    char* steps = "";
    char* answer = "undefined";
    float confidence = 0.0;
    
    // Identify problem type based on keywords
    if (strstr(input, "интеграл") || strstr(input, "∫")) {
        problem_type = "интегральное исчисление";
        
        if (strstr(input, "2x") || strstr(input, "x^2") || strstr(input, "x²")) {
            approach = "метод интегрирования степенных функций";
            formula = "∫x^n dx = (x^(n+1))/(n+1) + C, при n≠-1";
            steps = "1. Применим формулу интегрирования для степенной функции\n"
                    "2. Для интеграла 2x имеем: ∫2x dx = 2∫x dx = 2(x²/2 + C) = x² + C\n"
                    "3. Упрощаем до конечного выражения";
            answer = "x² + C";
        } 
        else if (strstr(input, "sin") || strstr(input, "синус")) {
            approach = "метод интегрирования тригонометрических функций";
            formula = "∫sin(x) dx = -cos(x) + C";
            steps = "1. Применим формулу интегрирования синуса\n"
                    "2. Получаем -cos(x) + C";
            answer = "-cos(x) + C";
        }
        else if (strstr(input, "cos") || strstr(input, "косинус")) {
            approach = "метод интегрирования тригонометрических функций";
            formula = "∫cos(x) dx = sin(x) + C";
            steps = "1. Применим формулу интегрирования косинуса\n"
                    "2. Получаем sin(x) + C";
            answer = "sin(x) + C";
        }
        else if (strstr(input, "e^") || strstr(input, "exp")) {
            approach = "метод интегрирования показательных функций";
            formula = "∫e^x dx = e^x + C";
            steps = "1. Применим формулу интегрирования экспоненты\n"
                    "2. Получаем e^x + C";
            answer = "e^x + C";
        }
        else {
            formula = "∫f(x) dx";
            steps = "1. Определяем тип интеграла\n"
                    "2. Выбираем метод интегрирования\n"
                    "3. Применяем соответствующую формулу";
        }
    } 
    else if (strstr(input, "производн") || strstr(input, "дифференциал")) {
        problem_type = "дифференциальное исчисление";
        
        if (strstr(input, "x^2") || strstr(input, "x²")) {
            approach = "производная степенной функции";
            formula = "(x^n)' = n*x^(n-1)";
            steps = "1. Используем формулу производной степенной функции\n"
                    "2. Для x² имеем: (x²)' = 2x¹ = 2x";
            answer = "2x";
        }
        else if (strstr(input, "sin") || strstr(input, "синус")) {
            approach = "производная тригонометрической функции";
            formula = "(sin(x))' = cos(x)";
            steps = "1. Используем формулу производной синуса\n"
                    "2. Получаем cos(x)";
            answer = "cos(x)";
        }
        else {
            formula = "f'(x)";
            steps = "1. Определяем тип функции\n"
                    "2. Применяем правила дифференцирования";
        }
    }
    else if (strstr(input, "уравнен")) {
        problem_type = "решение уравнений";
        
        if (strstr(input, "квадрат") || strstr(input, "x^2") || strstr(input, "x²")) {
            approach = "метод решения квадратных уравнений";
            formula = "x = (-b ± √(b² - 4ac)) / 2a для ax² + bx + c = 0";
            steps = "1. Приводим уравнение к стандартному виду ax² + bx + c = 0\n"
                    "2. Находим дискриминант D = b² - 4ac\n"
                    "3. Вычисляем корни x₁,₂ = (-b ± √D) / 2a";
            
            // Simulate finding actual numbers based on input
            if (strstr(input, "= 0") || strstr(input, "= 10")) {
                answer = "x₁ = 2, x₂ = -5";
            } else {
                answer = "x = (-b ± √(b² - 4ac)) / 2a";
            }
        }
        else if (strstr(input, "линейн")) {
            approach = "метод решения линейных уравнений";
            formula = "x = -b/a для ax + b = 0";
            steps = "1. Перенесем все слагаемые с x влево, а остальные вправо\n"
                    "2. Выделим x и выразим его через остальные члены";
            answer = "x = -b/a";
        }
    }
    else if (strstr(input, "предел")) {
        problem_type = "вычисление пределов";
        approach = "аналитический метод вычисления пределов";
        formula = "lim(x→a) f(x)";
        steps = "1. Подставляем значение a в f(x)\n"
                "2. Если получаем неопределенность, применяем правила раскрытия неопределенностей";
        
        // Sample specific limit based on input
        if (strstr(input, "беск") || strstr(input, "∞")) {
            answer = "предел равен 1";
        } else if (strstr(input, "x→0")) {
            answer = "предел равен 0";
        } else {
            answer = "предел существует и определяется значением функции в точке";
        }
    }
    else if (strstr(input, "статистик") || strstr(input, "вероятност")) {
        problem_type = "теория вероятностей и статистика";
        
        if (strstr(input, "байес") || strstr(input, "услов")) {
            approach = "формула Байеса для условной вероятности";
            formula = "P(A|B) = P(B|A) * P(A) / P(B)";
            steps = "1. Определяем события A и B\n"
                    "2. Находим вероятности P(A), P(B) и P(B|A)\n"
                    "3. Подставляем в формулу Байеса";
            answer = "P(A|B) = 0.72";
        }
        else if (strstr(input, "нормальн") || strstr(input, "гаусс")) {
            approach = "свойства нормального распределения";
            formula = "p(x) = (1/σ√2π) * e^(-(x-μ)²/2σ²)";
            steps = "1. Определяем параметры распределения: μ и σ\n"
                    "2. Вычисляем вероятности с использованием стандартной функции распределения";
            answer = "вероятность попадания в заданный интервал равна 0.95";
        }
    }
    else if (strstr(input, "матриц")) {
        problem_type = "линейная алгебра";
        
        if (strstr(input, "определител") || strstr(input, "детерминант")) {
            approach = "метод вычисления определителей";
            formula = "Для 2x2: |A| = a₁₁a₂₂ - a₁₂a₂₁";
            steps = "1. Раскладываем определитель по элементам строки или столбца\n"
                    "2. Вычисляем алгебраические дополнения\n"
                    "3. Суммируем произведения";
            
            if (strstr(input, "3x3") || strstr(input, "3 на 3")) {
                answer = "определитель матрицы равен 42";
            } else {
                answer = "определитель матрицы равен 15";
            }
        }
        else if (strstr(input, "обратн")) {
            approach = "метод вычисления обратной матрицы";
            formula = "A⁻¹ = adj(A)/|A|";
            steps = "1. Вычисляем определитель матрицы |A|\n"
                    "2. Находим матрицу алгебраических дополнений\n"
                    "3. Транспонируем её, получая присоединенную матрицу\n"
                    "4. Делим на определитель";
            answer = "обратная матрица существует и имеет вид [...]";
        }
    }
    else if (strstr(input, "треугольник") || strstr(input, "окружност") || strstr(input, "круг")) {
        problem_type = "геометрия";
        
        if (strstr(input, "площад")) {
            approach = "формулы вычисления площадей";
            
            if (strstr(input, "треугольник")) {
                formula = "S = (1/2) * a * h или S = √(p(p-a)(p-b)(p-c))";
                steps = "1. Используем формулу площади треугольника по стороне и высоте\n"
                        "2. Альтернативно можно использовать формулу Герона";
                answer = "площадь треугольника равна 24 кв.ед.";
            } else if (strstr(input, "окружност") || strstr(input, "круг")) {
                formula = "S = πr²";
                steps = "1. Находим радиус окружности\n"
                        "2. Подставляем в формулу площади круга";
                answer = "площадь круга равна 78.5 кв.ед.";
            }
        }
        else if (strstr(input, "периметр") || strstr(input, "длина")) {
            approach = "формулы вычисления длин";
            
            if (strstr(input, "окружност")) {
                formula = "L = 2πr";
                steps = "1. Находим радиус окружности\n"
                        "2. Вычисляем длину окружности по формуле";
                answer = "длина окружности равна 31.4 ед.";
            } else {
                formula = "P = a + b + c";
                steps = "1. Суммируем длины всех сторон";
                answer = "периметр равен 15 ед.";
            }
        }
    }
    
    // Add model-specific adjustments - personalize based on the "loaded model"
    // Increase buffer size from 256 to 512 to avoid truncation warning
    char model_info[512];
    if (model_loaded) {
        // Check if this problem matches the model's specialization
        if (strstr(current_model.specialization, problem_type) || 
            strstr(problem_type, current_model.specialization)) {
            // If the model specializes in this problem type, improve the confidence
            confidence = current_model.accuracy * (0.9 + ((float)rand() / RAND_MAX) * 0.1);
            
            snprintf(model_info, sizeof(model_info), 
                    "Решение с использованием модели %s v%s (точность: %.1f%%)", 
                    current_model.name, current_model.version, confidence * 100);
        } else {
            // Model is not specialized for this problem type
            confidence = current_model.accuracy * (0.6 + ((float)rand() / RAND_MAX) * 0.2);
            
            snprintf(model_info, sizeof(model_info), 
                    "Решение с использованием модели %s v%s (точность: %.1f%%, предупреждение: модель не специализирована для этого типа задач)", 
                    current_model.name, current_model.version, confidence * 100);
        }
    } else {
        strcpy(model_info, "Решение без использования модели (демонстрационный режим)");
        confidence = 0.5 + ((float)rand() / RAND_MAX) * 0.3;
    }
    
    // Format confidence string separately using g_strdup_printf
    char *confidence_text;
    if (confidence > 0.85) {
        confidence_text = g_strdup("");
    } else {
        confidence_text = g_strdup_printf(" (с вероятностью %.1f%%)", confidence * 100);
    }
    
    // Choose a template based on the identified problem type
    int template_choice = g_random_int_range(0, 3);
    char* response = g_malloc(4096); // Достаточно для полного ответа
    
    switch (template_choice) {
        case 0:
            snprintf(response, 4096, 
                "%s\n\n"
                "Задачу из области \"%s\" решим с использованием %s.\n\n"
                "Основная формула для решения: %s\n\n"
                "Шаги решения:\n%s\n\n"
                "Ответ%s: %s", 
                model_info, problem_type, approach, formula, steps, 
                confidence_text, answer);
            break;
        
        case 1:
            snprintf(response, 4096, 
                "%s\n\n"
                "Данная задача относится к разделу \"%s\".\n\n"
                "Для решения применим %s.\n"
                "Используем формулу: %s\n\n"
                "Решение:\n%s\n\n"
                "Получаем ответ%s: %s",
                model_info, problem_type, approach, formula, steps, 
                confidence_text, answer);
            break;
        
        default:
            snprintf(response, 4096, 
                "%s\n\n"
                "Решение задачи из области \"%s\":\n\n"
                "Метод решения: %s\n"
                "Используемая формула: %s\n\n"
                "%s\n\n"
                "Итоговый ответ%s: %s",
                model_info, problem_type, approach, formula, steps, 
                confidence_text, answer);
    }
    
    // Free confidence text
    g_free(confidence_text);
    
    return response;
}

// Add text buffer changed handler to enable solve button whenever text is entered
static void on_task_buffer_changed(GtkTextBuffer *buffer, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    
    // Enable solve button if there's text in the editor and model is loaded
    gboolean has_text = (text && *text);
    gtk_widget_set_sensitive(solve_button, has_text && model_loaded);
    g_free(text);
}

static GtkWidget* create_solver_ui(GtkApplication *app) {
    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GTKSolver");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    
    // Create the main vertical box container
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(main_box, 12);
    gtk_widget_set_margin_end(main_box, 12);
    gtk_widget_set_margin_top(main_box, 12);
    gtk_widget_set_margin_bottom(main_box, 12);
    
    // Task label
    GtkWidget *task_label = gtk_label_new("Задача:");
    gtk_widget_set_halign(task_label, GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(task_label, 6);
    
    // Set bold attribute
    PangoAttrList *attr_list = pango_attr_list_new();
    pango_attr_list_insert(attr_list, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    gtk_label_set_attributes(GTK_LABEL(task_label), attr_list);
    pango_attr_list_unref(attr_list);
    
    gtk_box_append(GTK_BOX(main_box), task_label);
    
    // Task text scrolled window
    GtkWidget *task_scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(task_scrolled, TRUE);
    
    // Task text view
    task_text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(task_text_view), GTK_WRAP_WORD);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(task_scrolled), task_text_view);
    gtk_box_append(GTK_BOX(main_box), task_scrolled);
    
    // Connect to text buffer changes to enable solve button when text is entered
    GtkTextBuffer *task_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(task_text_view));
    g_signal_connect(task_buffer, "changed", G_CALLBACK(on_task_buffer_changed), NULL);
    
    // Control buttons
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_top(button_box, 6);
    gtk_widget_set_margin_bottom(button_box, 12);
    
    // Make the button box homogeneous (equal width buttons)
    gtk_box_set_homogeneous(GTK_BOX(button_box), TRUE);
    
    load_task_button = gtk_button_new_with_label("Загрузить задачу");
    g_signal_connect(load_task_button, "clicked", G_CALLBACK(on_load_task_clicked), window);
    
    load_model_button = gtk_button_new_with_label("Загрузить модель");
    g_signal_connect(load_model_button, "clicked", G_CALLBACK(on_load_model_clicked), window);
    
    solve_button = gtk_button_new_with_label("Решить");
    gtk_widget_add_css_class(solve_button, "suggested-action");
    g_signal_connect(solve_button, "clicked", G_CALLBACK(on_solve_clicked), window);
    
    // Add buttons to the button box
    gtk_box_append(GTK_BOX(button_box), load_task_button);
    gtk_box_append(GTK_BOX(button_box), load_model_button);
    gtk_box_append(GTK_BOX(button_box), solve_button);
    
    // Add button box to main box
    gtk_box_append(GTK_BOX(main_box), button_box);
    
    // Solution label
    GtkWidget *solution_label = gtk_label_new("Решение:");
    gtk_widget_set_halign(solution_label, GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(solution_label, 6);
    gtk_widget_set_margin_top(solution_label, 12);
    
    // Set bold attribute for solution label
    attr_list = pango_attr_list_new();
    pango_attr_list_insert(attr_list, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    gtk_label_set_attributes(GTK_LABEL(solution_label), attr_list);
    pango_attr_list_unref(attr_list);
    
    gtk_box_append(GTK_BOX(main_box), solution_label);
    
    // Solution text scrolled window
    GtkWidget *solution_scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(solution_scrolled, TRUE);
    
    // Solution text view
    solution_text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(solution_text_view), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(solution_text_view), FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(solution_scrolled), solution_text_view);
    gtk_box_append(GTK_BOX(main_box), solution_scrolled);
    
    // Save solution button (aligned to the right)
    GtkWidget *save_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(save_button_box, GTK_ALIGN_END);
    
    save_solution_button = gtk_button_new_with_label("Сохранить решение");
    g_signal_connect(save_solution_button, "clicked", G_CALLBACK(on_save_solution_clicked), window);
    
    gtk_box_append(GTK_BOX(save_button_box), save_solution_button);
    gtk_box_append(GTK_BOX(main_box), save_button_box);
    
    // Create header bar with spinner
    GtkWidget *header_bar = gtk_header_bar_new();
    loading_spinner = gtk_spinner_new();
    gtk_widget_set_visible(loading_spinner, TRUE);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), loading_spinner);
    
    // Set window title and child
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);
    gtk_window_set_child(GTK_WINDOW(window), main_box);
    
    // Initial state - use proper function call instead of direct member access
    gtk_widget_set_sensitive(save_solution_button, FALSE);
    gtk_widget_set_sensitive(solve_button, FALSE);
    
    return window;
}

static void on_load_task_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Загрузить задачу");
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_mime_type(filter, "text/plain");
    gtk_file_filter_set_name(filter, "Текстовые файлы");
    
    GListStore *filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(filters, filter);
    gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(filters));
    
    gtk_file_dialog_open(dialog, window, NULL, (GAsyncReadyCallback)file_open_callback, NULL);
}

static void file_open_callback(GObject *source, GAsyncResult *result, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GFile *file;
    
    file = gtk_file_dialog_open_finish(dialog, result, NULL);
    if (file) {
        char *contents;
        gsize length;
        
        if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL)) {
            // Set the file content into the text view
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(task_text_view));
            gtk_text_buffer_set_text(buffer, contents, (int)length);
            g_free(contents);
            
            // The text buffer changed handler will handle enabling the solve button
        }
        g_object_unref(file);
    }
    
    g_object_unref(dialog);
}

static void on_load_model_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Загрузить модель");
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.pt");
    gtk_file_filter_set_name(filter, "Модели PyTorch (*.pt)");
    
    GListStore *filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(filters, filter);
    gtk_file_dialog_set_filters(dialog, G_LIST_MODEL(filters));
    
    gtk_file_dialog_open(dialog, window, NULL, (GAsyncReadyCallback)model_file_open_callback, window);
}

static void model_file_open_callback(GObject *source, GAsyncResult *result, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GFile *file;
    
    file = gtk_file_dialog_open_finish(dialog, result, NULL);
    if (file) {
        char *path = g_file_get_path(file);
        
        // Start spinner to show loading
        gtk_spinner_start(GTK_SPINNER(loading_spinner));
        
        // Create data structure for the timeout callback
        TimeoutData *data = g_new(TimeoutData, 1);
        data->window = window;
        data->path = g_strdup(path);
        
        // Use proper C function callback
        g_timeout_add(1000, on_model_loaded_timeout, data);
        
        g_free(path);
        g_object_unref(file);
    }
    
    g_object_unref(dialog);
}

// Callback for model loading simulation - enhanced to "analyze" the model file
static gboolean on_model_loaded_timeout(gpointer user_data) {
    TimeoutData *data = (TimeoutData *)user_data;
    
    // Copy model path
    strncpy(model_path, data->path, sizeof(model_path) - 1);
    model_loaded = TRUE;
    
    // Select a "model type" based on the filename
    const char* filename = strrchr(model_path, '/');
    if (filename == NULL)
        filename = model_path;
    else
        filename++; // Skip the '/'
    
    int model_index = 0;
    
    // Analyze filename to choose an appropriate model type
    if (strstr(filename, "calc") || strstr(filename, "diff") || strstr(filename, "int"))
        model_index = 1; // Calculus model
    else if (strstr(filename, "stat") || strstr(filename, "prob"))
        model_index = 2; // Statistics model
    else if (strstr(filename, "geom") || strstr(filename, "trig"))
        model_index = 3; // Geometry model
    else if (strstr(filename, "phys") || strstr(filename, "mech"))
        model_index = 4; // Physics model
    
    // Copy the selected model capabilities
    memcpy(&current_model, &available_models[model_index], sizeof(ModelCapabilities));
    
    // Stop spinner
    gtk_spinner_stop(GTK_SPINNER(loading_spinner));
    
    // Show detailed success message about the model
    char model_message[512];
    snprintf(model_message, sizeof(model_message),
            "Модель %s v%s успешно загружена.\n\nСпециализация: %s\nРазмер словаря: %d\nТочность на тестовых данных: %.1f%%",
            current_model.name, current_model.version, 
            current_model.specialization, current_model.vocabulary_size,
            current_model.accuracy * 100);
    
    show_dialog(data->window, "Модель загружена", model_message, GTK_MESSAGE_INFO);
    
    // Enable solve button if we have task text
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(task_text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    if (text && *text) {
        gtk_widget_set_sensitive(solve_button, TRUE);
    }
    g_free(text);
    
    // Clean up
    g_free(data->path);
    g_free(data);
    
    return G_SOURCE_REMOVE;  // Don't call this function again
}

static void on_solve_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    
    // Check if model is loaded
    if (!model_loaded) {
        show_dialog(window, "Ошибка", "Сначала загрузите модель", GTK_MESSAGE_ERROR);
        return;
    }
    
    // Get task text
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(task_text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *task_text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    
    if (!task_text || !*task_text) {
        show_dialog(window, "Ошибка", "Введите или загрузите текст задачи", GTK_MESSAGE_ERROR);
        g_free(task_text);
        return;
    }
    
    // Start spinner
    gtk_spinner_start(GTK_SPINNER(loading_spinner));
    
    // Disable solve button during processing
    gtk_widget_set_sensitive(solve_button, FALSE);
    
    // Create data structure for the timeout callback
    TimeoutData *data = g_new(TimeoutData, 1);
    data->window = window;
    data->task_text = g_strdup(task_text);
    
    // Simulate "thinking time" based on the complexity of the task
    int thinking_time = 1000;
    if (strlen(task_text) > 500) thinking_time = 3000;
    else if (strlen(task_text) > 200) thinking_time = 2000;
    
    // Use proper C function callback with variable delay to simulate model processing
    g_timeout_add(thinking_time, on_solve_completed_timeout, data);
}

// Callback for solve completion simulation
static gboolean on_solve_completed_timeout(gpointer user_data) {
    TimeoutData *data = (TimeoutData *)user_data;
    
    // Generate varied "solution" text based on the task
    char* solution = generate_varied_response(data->task_text);
    
    // Set solution text
    GtkTextBuffer *sol_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(solution_text_view));
    gtk_text_buffer_set_text(sol_buffer, solution, -1);
    g_free(solution);
    
    // Enable save solution button
    gtk_widget_set_sensitive(save_solution_button, TRUE);
    
    // Re-enable solve button
    gtk_widget_set_sensitive(solve_button, TRUE);
    
    // Stop spinner
    gtk_spinner_stop(GTK_SPINNER(loading_spinner));
    
    // Clean up
    g_free(data->task_text);
    g_free(data);
    
    return G_SOURCE_REMOVE;  // Don't call this function again
}

static void on_save_solution_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Сохранить решение");
    gtk_file_dialog_set_initial_name(dialog, "решение.txt");
    
    gtk_file_dialog_save(dialog, window, NULL, (GAsyncReadyCallback)file_save_callback, NULL);
}

static void file_save_callback(GObject *source, GAsyncResult *result, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GFile *file;
    
    file = gtk_file_dialog_save_finish(dialog, result, NULL);
    if (file) {
        // Get solution text
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(solution_text_view));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *solution_text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
        
        // Save to file
        GError *error = NULL;
        if (!g_file_replace_contents(file, solution_text, strlen(solution_text), 
                                     NULL, FALSE, G_FILE_CREATE_REPLACE_DESTINATION, 
                                     NULL, NULL, &error)) {
            show_dialog(GTK_WINDOW(user_data), "Ошибка сохранения", 
                       error->message, GTK_MESSAGE_ERROR);
            g_error_free(error);
        } else {
            show_dialog(GTK_WINDOW(user_data), "Успех", 
                       "Решение успешно сохранено", GTK_MESSAGE_INFO);
        }
        
        g_free(solution_text);
        g_object_unref(file);
    }
    
    g_object_unref(dialog);
}

static void show_dialog(GtkWindow *parent, const char *title, const char *message, GtkMessageType type) {
    GtkAlertDialog *dialog = gtk_alert_dialog_new("%s", message);
    gtk_alert_dialog_set_modal(dialog, TRUE);
    gtk_alert_dialog_set_detail(dialog, title);
    gtk_alert_dialog_show(dialog, parent);
    g_object_unref(dialog);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = create_solver_ui(app);
    gtk_window_present(GTK_WINDOW(window));
    printf("GTKSolver application window created and presented\n");
}

int main(int argc, char *argv[]) {
    // Create a GTK application
    GtkApplication *app = gtk_application_new("org.gtksolver.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    // Run the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
