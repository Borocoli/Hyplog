# Hyplog

Hyplog is a lightweight, terminal-based tool designed to document and track experiments

Hyplog is designed for dual-mode utility: 
* interactively by humans
* non-interactively by automated scripts and AI agent workflows.

---

## 🧠 Data Structure

Every entry follows this logical flow:

1.  **Hypothesis**: The underlying question being tested (the "Why").
2.  **Experiment**: The methodology, parameters, and steps taken (the "How").
3.  **Results**: The interpretation of the results from the experiment (the "What").

---

## 🛠 Building the Project


### Default Build
```bash
mkdir build && cd build
cmake ..
make
```

### Customized Build
Use the `-D` flag to override the default configuration:

| Flag | Description | Default |
| :--- | :--- | :--- |
| `I_VAL` | **Interactivity Level**: Set to `1` for interactive human use; set to `0` for non-interactive use (scripts/AI agents). | `0` |
| `TLEN_VAL` | **Max Text Length**: The maximum character count allowed in each field. Reducing this is highly recommended when using AI agents to prevent context window clutter. | `512` |
| `PATH_VAL` | **Storage Path**: The directory where all experiment logs will be stored. | `~/.hyplog/` |
| `LAST_VAL` | **Last Keyword Buffer**: Determines how many recent entries are retrieved when using the `last` keyword. | `5` |

**Example: Building for an AI Agent**
If you are integrating Hyplog into an AI agent, you may want a non-interactive mode with shorter text limits to save tokens:
```bash
cmake -DI_VAL=0 -DTLEN_VAL=256 -DPATH_VAL="/tmp/ai_logs/" ..
make
```

---

## 🚀 Usage

### 1. Creating an Experiment
The `create` subcommand records a new experiment.

**Non-Interactive Mode (Recommended for Scripts/AI):**
Pass the files containing your hypothesis, experiment details, and results as arguments.
```bash
# Usage: hyplog create <hyp.txt> <exp.txt> <res.txt>
hyplog create hypothesis.txt method.txt results.txt
```

**Interactive Mode (Human Use):**
If compiled with `I_VAL=1`, running `hyplog create` will prompt you step-by-step to type in your data.

### 2. Reading Experiments
The `read` subcommand allows you to retrieve historical data.

**Syntax Examples:**
*   **Specific ID**: View a single experiment by its unique ID.
    ```bash
    hyplog read 42
    ```
*   **Range of IDs**: View a specific window of experiments.
    ```bash
    hyplog read 10-20
    ```
*   **Recent Entries (Negative Index)**: View the last $n$ entries.
    ```bash
    hyplog read -3  # Shows the 3 most recent experiments
    ```
*   **The `last` Keyword**: View the most recent entries based on the `LAST_VAL` compile-time setting.
    ```bash
    hyplog read last
    ```

---

## ⚠️ Limitations

*   **POSIX Requirement**: Hyplog is limited to **POSIX-compliant systems** (Linux, macOS, etc.). It relies on the `off_t` data type for file positioning and system-level file handling, which may not behave identically on Windows environments.

