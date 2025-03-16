using GLib;

public class TaskProcessor : Object {
    // External C function declarations
    [CCode (cname = "process_task_with_model")]
    private extern string? process_task_with_model_c(string task_text, void* model_ptr);
    
    public TaskProcessor() {
        // Initialize
    }
    
    public async string process_task(string task_text, void* model) throws Error {
        SourceFunc callback = process_task.callback;
        string? result = null;
        
        ThreadFunc<bool> run = () => {
            result = process_task_with_model_c(task_text, model);
            Idle.add((owned) callback);
            return true;
        };
        
        try {
            Thread<bool> thread = new Thread<bool>("task-process", (owned) run);
            yield;
            thread.join();
            
            if (result == null) {
                throw new IOError.FAILED("Не удалось обработать задачу");
            }
            
            return result;
        } catch (Error e) {
            throw new IOError.FAILED("Ошибка при обработке задачи: " + e.message);
        }
    }
}
