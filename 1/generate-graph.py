import json
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker

with open("report_get_opt.json") as f:
    data = json.load(f)["benchmarks"]

df = pd.DataFrame(data)

# Имя функции без суффикса /N
df["base_name"] = df["name"].str.split("/").str[0]
df["elements"] = df["name"].str.extract(r"/(\d+)$").astype(int)

# Разбираем части имени бенчмарка
parts = df["base_name"].str.split("_", expand=True)
df["operation"] = parts[2]      # Set / Get / Delete
df["scenario"] = parts[3]       # No / Many / Random (первые слова)
df["scenario_detail"] = parts[4]  # Collisions, если есть

# print(df[["name", "base_name", "operation",
#       "scenario", "scenario_detail", "elements"]])

# Объединяем сценарий
df["scenario"] = df["scenario"] + "_" + df["scenario_detail"].fillna("")

# Перевод для читаемости
df["operation"] = df["operation"].replace({
    "Set": "Вставка",
    "Get": "Поиск",
    "Delete": "Удаление"
})
df["scenario"] = df["scenario"].replace({
    "No_Collisions": "Без коллизий",
    "Many_Collisions": "Много коллизий",
    "Random_Collisions": "Случайный сценарий"
})

colors = {
    "Без коллизий": "green",
    "Много коллизий": "red",
    "Случайный сценарий": "blue"
}

# === Рисуем три графика ===
for operation in ["Вставка", "Поиск", "Удаление"]:
    subset = df[df["operation"] == operation]
    plt.figure(figsize=(8, 5))
    for scenario, color in colors.items():
        # print(subset["scenario"])
        s = subset[subset["scenario"] == scenario]
        if not s.empty:
            plt.plot(s["elements"], s["real_time"],
                     label=scenario, color=color, marker="o")

    plt.title(f"HashMap — {operation}")
    plt.xlabel("Количество элементов")
    ax = plt.gca()
    ax.xaxis.set_major_formatter(mticker.FuncFormatter(
        lambda x, _: f"{int(x/1e5)} " if x >= 1e5 else f"{int(x/1e3)}K"
    ))
    plt.xscale("log")
    plt.ylabel("Время (нс на итерацию)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()
