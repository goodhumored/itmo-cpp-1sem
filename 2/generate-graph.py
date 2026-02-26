import json
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker

with open("report.json") as f:
    data = json.load(f)["benchmarks"]

df = pd.DataFrame(data)

df["base_name"] = df["name"].str.split("/").str[0]
df["size"] = df["name"].str.extract(r"/(\d+)$").astype(int)

parts = df["base_name"].str.split("_", expand=True)
df["method"] = parts[2]
df["scenario"] = parts[3]

df["method"] = df["method"].replace({
    "Naive": "Наивный",
    "SIMD": "SIMD"
})
df["scenario"] = df["scenario"].replace({
    "Random": "Случайные данные",
    "Uniform": "Однородные данные",
    "Gradient": "Градиент"
})

colors = {
    "Наивный": "blue",
    "SIMD": "green"
}

for scenario in ["Случайные данные", "Однородные данные", "Градиент"]:
    subset = df[df["scenario"] == scenario]
    plt.figure(figsize=(10, 6))
    
    for method, color in colors.items():
        s = subset[subset["method"] == method]
        if not s.empty:
            plt.plot(s["size"], s["real_time"] / 1e6, 
                     label=method, color=color, marker="o", linewidth=2)
    
    plt.title(f"Гистограмма яркости — {scenario}")
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
    plt.savefig(f"bench_{scenario.replace(' ', '_').lower()}.png", dpi=150)
    plt.close()

plt.figure(figsize=(10, 6))
large_data = df[df["size"] == df["size"].max()]

scenarios = large_data["scenario"].unique()
methods = ["Наивный", "SIMD"]
x = range(len(scenarios))
width = 0.35

for i, method in enumerate(methods):
    times = []
    for scenario in scenarios:
        row = large_data[(large_data["scenario"] == scenario) & (large_data["method"] == method)]
        if not row.empty:
            times.append(row["real_time"].values[0] / 1e6)
        else:
            times.append(0)
    
    plt.bar([xi + i * width for xi in x], times, width, label=method, 
            color=colors[method], alpha=0.8)

plt.title("Сравнение Naive vs SIMD (16 МБ)")
plt.xlabel("Сценарий")
plt.ylabel("Время (мс)")
plt.xticks([xi + width/2 for xi in x], scenarios, rotation=15)
plt.legend()
plt.grid(True, axis='y', alpha=0.3)
plt.tight_layout()
plt.savefig("bench_comparison.png", dpi=150)
plt.close()

print("Графики сохранены:")
print("- bench_случайные_данные.png")
print("- bench_однородные_данные.png") 
print("- bench_градиент.png")
print("- bench_comparison.png")
