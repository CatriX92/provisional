import os
import re
import pandas as pd
from matplotlib import pyplot as plt

carpeta_logs = "code\sorting\data\measurements"
carpeta_plots = "code\sorting\data\plots"
os.makedirs(carpeta_plots, exist_ok=True)

regex_elementos = r"elementos: (\d+)"
regex_tiempo = r"tiempo: ([\d.eE+-]+) s"

datos = []

for archivo in os.listdir(carpeta_logs):
    if archivo.endswith(".txt"):
        ruta = os.path.join(carpeta_logs, archivo)
        with open(ruta, "r", encoding="utf-8") as f:
            contenido = f.read()
            elementos = re.findall(regex_elementos, contenido)
            tiempos = re.findall(regex_tiempo, contenido)
            for n, t in zip(elementos, tiempos):
                datos.append({"Archivo": archivo, "n": int(n), "Tiempo (s)": float(t)})

df = pd.DataFrame(datos)

df_grouped = df.groupby("n")["Tiempo (s)"].mean().reset_index()

# 🎨 Generar gráfico
plt.figure(figsize=(10, 6))
plt.plot(df_grouped["n"], df_grouped["Tiempo (s)"], marker="o", linestyle="-", color="teal")
plt.title("Tiempo promedio de ejecución según n")
plt.xlabel("Cantidad de elementos (n)")
plt.ylabel("Tiempo promedio (s)")
plt.grid(True)
plt.tight_layout()

ruta_png = os.path.join(carpeta_plots, "tiempo_vs_n.png")
plt.savefig(ruta_png)
plt.close()

print(f"✅ Gráfico guardado en: {ruta_png}")