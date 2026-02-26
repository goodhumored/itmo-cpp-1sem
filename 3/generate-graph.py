import json
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker

with open("report.json") as f:
    data = json.load(f)["benchmarks"]

df = pd.DataFrame(data)

hist_df = df[df["name"].str.contains("Histogram")].copy()
hist_df["size"] = hist_df["name"].str.extract(r"/(\d+)$").astype(int)

def get_method(name):
    if "Naive" in name:
        return "Naive (1 поток)"
    elif "Parallel_2" in name:
        return "Parallel (2 потока)"
    elif "Parallel_4" in name:
        return "Parallel (4 потока)"
    elif "Parallel_8" in name:
        return "Parallel (8 потоков)"
    elif "Parallel_Auto" in name:
        return "Parallel (auto)"
    return "Unknown"

hist_df["method"] = hist_df["name"].apply(get_method)

colors = {
    "Naive (1 поток)": "blue",
    "Parallel (2 потока)": "orange",
    "Parallel (4 потока)": "green",
    "Parallel (8 потоков)": "red",
    "Parallel (auto)": "purple"
}

plt.figure(figsize=(12, 7))
for method in ["Naive (1 поток)", "Parallel (2 потока)", "Parallel (4 потока)", 
               "Parallel (8 потоков)", "Parallel (auto)"]:
    subset = hist_df[hist_df["method"] == method]
    if not subset.empty:
        plt.plot(subset["size"], subset["real_time"] / 1e6,
                 label=method, color=colors[method], marker="o", linewidth=2)

plt.title("Гистограмма яркости — сравнение многопоточных версий")
plt.xlabel("Размер изображения (байт)")
plt.ylabel("Время (мс)")
plt.xscale("log")
plt.yscale("log")

ax = plt.gca()
ax.xaxis.set_major_formatter(mticker.FuncFormatter(
    lambda x, _: f"{int(x/1e6)}M" if x >= 1e6 else f"{int(x/1e3)}K"
))

plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("bench_histogram_parallel.png", dpi=150)
plt.close()

plt.figure(figsize=(10, 6))
large_data = hist_df[hist_df["size"] == hist_df["size"].max()]

methods = ["Naive (1 поток)", "Parallel (2 потока)", "Parallel (4 потока)", 
           "Parallel (8 потоков)", "Parallel (auto)"]
times = []
for method in methods:
    row = large_data[large_data["method"] == method]
    if not row.empty:
        times.append(row["real_time"].values[0] / 1e6)
    else:
        times.append(0)

bars = plt.bar(range(len(methods)), times, color=[colors[m] for m in methods], alpha=0.8)
plt.title("Время обработки 64 МБ изображения")
plt.xlabel("Метод")
plt.ylabel("Время (мс)")
plt.xticks(range(len(methods)), [m.replace(" (", "\n(") for m in methods], fontsize=9)
plt.grid(True, axis='y', alpha=0.3)

for bar, time in zip(bars, times):
    plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5,
             f'{time:.1f}ms', ha='center', va='bottom', fontsize=9)

plt.tight_layout()
plt.savefig("bench_histogram_comparison.png", dpi=150)
plt.close()

naive_time = large_data[large_data["method"] == "Naive (1 поток)"]["real_time"].values[0]
plt.figure(figsize=(10, 6))

speedups = []
methods_for_speedup = ["Parallel (2 потока)", "Parallel (4 потока)", 
                       "Parallel (8 потоков)", "Parallel (auto)"]
for method in methods_for_speedup:
    row = large_data[large_data["method"] == method]
    if not row.empty:
        speedups.append(naive_time / row["real_time"].values[0])
    else:
        speedups.append(0)

bars = plt.bar(range(len(methods_for_speedup)), speedups, 
               color=[colors[m] for m in methods_for_speedup], alpha=0.8)
plt.axhline(y=1, color='blue', linestyle='--', label='Baseline (Naive)')
plt.title("Ускорение относительно Naive (64 МБ)")
plt.xlabel("Метод")
plt.ylabel("Ускорение (раз)")
plt.xticks(range(len(methods_for_speedup)), 
           [m.replace(" (", "\n(") for m in methods_for_speedup], fontsize=9)
plt.grid(True, axis='y', alpha=0.3)

for bar, speedup in zip(bars, speedups):
    plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.1,
             f'{speedup:.1f}x', ha='center', va='bottom', fontsize=10)

plt.tight_layout()
plt.savefig("bench_speedup.png", dpi=150)
plt.close()

pool_df = df[df["name"].str.contains("ThreadPool")].copy()

batch_df = pool_df[pool_df["name"].str.contains("Batch")].copy()
batch_df["batch_size"] = batch_df["name"].str.extract(r"/(\d+)$").astype(int)

plt.figure(figsize=(10, 6))
plt.plot(batch_df["batch_size"], batch_df["real_time"] / 1e3, 
         marker="o", linewidth=2, color="green")
plt.title("ThreadPool — время выполнения batch задач")
plt.xlabel("Размер batch")
plt.ylabel("Время (мкс)")
plt.xscale("log")
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("bench_threadpool_batch.png", dpi=150)
plt.close()

print("Графики сохранены:")
print("- bench_histogram_parallel.png")
print("- bench_histogram_comparison.png")
print("- bench_speedup.png")
print("- bench_threadpool_batch.png")
