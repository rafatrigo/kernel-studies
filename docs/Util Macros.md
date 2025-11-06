#kernel #kernelDev 

# Kernel Log Severity

| **Macro**          | **Level (String)** | **Description**                                                                       |     |
| ------------------ | ------------------ | ------------------------------------------------------------------------------------- | --- |
| **`KERN_EMERG`**   | `<0>`              | **Emergency:** System is unusable. (Ex: Kernel Panic)                                 |     |
| **`KERN_ALERT`**   | `<1>`              | **Alert:** Action must be taken immediately.                                          |     |
| **`KERN_CRIT`**    | `<2>`              | **Critical:** Critical conditions. (Ex: Severe hardware error)                        |     |
| **`KERN_ERR`**     | `<3>`              | **Error:** Error conditions. Used to report failures.                                 |     |
| **`KERN_WARNING`** | `<4>`              | **Warning:** Warning conditions. Something is not ideal, but it works.                |     |
| **`KERN_NOTICE`**  | `<5>`              | **Notice:** Normal, but a notable condition.                                          |     |
| **`KERN_INFO`**    | `<6>`              | **Informational:** Informational messages. (Ex: "Driver loaded", "Network connected") |     |
| **`KERN_DEBUG`**   | `<7>`              | **Debug:** Debug-level messages.                                                      |     |

**Usage**
```c
printk(<macro> "message string");
```
