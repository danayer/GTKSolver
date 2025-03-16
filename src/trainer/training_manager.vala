using GLib;

public class TrainingManager : Object {
    // Signals for progress reporting
    public signal void progress_changed(double progress, string message);
    public signal void training_completed(bool success, string message);
    
    // External C function declarations for training
    [CCode (cname = "initialize_trainer")]
    private extern bool initialize_trainer_c(string corpus_path, double learning_rate);
    
    [CCode (cname = "run_training")]
    private extern bool run_training_c(string output_path, int epochs, int batch_size, bool* should_stop);
    
    [CCode (cname = "get_training_progress")]
    private extern double get_training_progress_c();
    
    [CCode (cname = "get_training_status")]
    private extern string get_training_status_c();
    
    private bool should_stop;
    private uint progress_timeout_id;
    
    public TrainingManager() {
        // Initialize
        should_stop = false;
    }
    
    public async void start_training(string corpus_path, string output_path, 
                                    int epochs, int batch_size, double learning_rate) throws Error {
        should_stop = false;
        
        // Start progress monitoring
        progress_timeout_id = Timeout.add(500, update_progress);
        
        SourceFunc callback = start_training.callback;
        
        ThreadFunc<bool> run = () => {
            // Initialize trainer with corpus
            if (!initialize_trainer_c(corpus_path, learning_rate)) {
                Idle.add(() => {
                    training_completed(false, "Не удалось инициализировать обучение с указанным корпусом");
                    Source.remove(progress_timeout_id);
                    callback();
                    return false;
                });
                return false;
            }
            
            // Run training
            bool success = run_training_c(output_path, epochs, batch_size, &should_stop);
            
            Idle.add(() => {
                if (success) {
                    training_completed(true, "Обучение успешно завершено");
                } else if (should_stop) {
                    training_completed(false, "Обучение прервано пользователем");
                } else {
                    training_completed(false, "Произошла ошибка в процессе обучения");
                }
                
                Source.remove(progress_timeout_id);
                callback();
                return false;
            });
            
            return success;
        };
        
        try {
            Thread<bool> thread = new Thread<bool>("training-thread", (owned) run);
            yield;
        } catch (Error e) {
            Source.remove(progress_timeout_id);
            throw new IOError.FAILED("Ошибка при запуске обучения: " + e.message);
        }
    }
    
    public void stop_training() {
        should_stop = true;
    }
    
    private bool update_progress() {
        double progress = get_training_progress_c();
        string status = get_training_status_c();
        progress_changed(progress, status);
        return true; // Continue updating
    }
}
