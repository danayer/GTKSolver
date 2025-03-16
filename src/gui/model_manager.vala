using GLib;

public class ModelManager : Object {
    private bool model_loaded = false;
    private string model_path = "";
    private string vocabulary_path = "";
    
    // External C function declarations
    [CCode (cname = "load_model_from_file")]
    private extern bool load_model_from_file_c(string model_path);
    
    [CCode (cname = "load_vocabulary_from_file")]
    private extern bool load_vocabulary_from_file_c(string vocab_path);
    
    public ModelManager() {
        // Initialize
    }
    
    public async void load_model(string path) throws Error {
        // Look for vocabulary file in the same directory
        string vocab_path = path.replace(".pt", ".vocab");
        
        SourceFunc callback = load_model.callback;
        
        ThreadFunc<bool> run = () => {
            bool success = load_model_from_file_c(path);
            if (success && FileUtils.test(vocab_path, FileTest.EXISTS)) {
                success = load_vocabulary_from_file_c(vocab_path);
            }
            
            Idle.add((owned) callback);
            return success;
        };
        
        try {
            Thread<bool> thread = new Thread<bool>("model-load", (owned) run);
            yield;
            
            bool success = thread.join();
            if (success) {
                model_loaded = true;
                model_path = path;
                vocabulary_path = vocab_path;
            } else {
                throw new IOError.FAILED("Не удалось загрузить модель");
            }
        } catch (Error e) {
            throw new IOError.FAILED("Ошибка при загрузке модели: " + e.message);
        }
    }
    
    public bool is_model_loaded() {
        return model_loaded;
    }
    
    public void* get_model() {
        // This function would return a pointer to the C++ model object
        // For simplicity in this Vala interface, we return void*
        // In a real implementation, there would be an external C function to get the model pointer
        return null;
    }
    
    public string get_model_path() {
        return model_path;
    }
}
