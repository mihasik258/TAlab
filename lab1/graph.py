import matplotlib.pyplot as plt
import pandas as pd

def plot_accumulated_time(files, labels):
    plt.figure(figsize=(10, 5))

    for file, label in zip(files, labels):
        df = pd.read_csv(file, delim_whitespace=True, names=["Count", "Time"])
        plt.plot(df["Count"], df["Time"], label=label, marker='o')

    plt.xlabel("Длина строк")
    plt.ylabel("Накопленное время (нс)")
    plt.title("Суммарное время обработки URL")
    plt.legend()
    plt.grid(True)
    plt.show()

plot_accumulated_time(
    ["timing_regex.txt", "timing_smc.txt", "timing_flex.txt"],
    ["Regex", "SMC", "Flex"]
)


