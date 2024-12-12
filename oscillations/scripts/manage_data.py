import pandas as pd
import numpy as np

def combine_driven_force_data():
    total_dfs = []
    
    for M in range(1, 3):
        for i in range(1, 18):
            csv_filepath = f"oscillations/data/M{M}/{i}.csv"
            data = pd.read_csv(csv_filepath, delimiter=";")
            data = data.dropna()
            cols = data.columns
            # print(f"INDEX {M}/{i}\n{cols[0]}, {cols[1]}, {cols[7]}, {cols[13]}\n=============================")
            
            time = data[cols[0]].str.replace(",", ".").astype("float")
            angle = data[cols[1]].str.replace(",", ".").astype("float")
            driven_angle = data[cols[7]].str.replace(",", ".").astype("float")
            voltage = data[cols[13]].str.replace(",", ".").astype("float")
            
            result = pd.concat([time, angle, driven_angle, voltage], axis=1)
            result.columns = [f"{M}time{i}", f"{M}angle{i}", f"{M}driven_angle{i}", f"{M}voltage{i}"]
            
            total_dfs.append(result)
            
    min_time = min([df.shape[0] for df in total_dfs])
    total_dfs = [df.iloc[:min_time] for df in total_dfs]
    combined_df = pd.concat(total_dfs, axis=1)
    combined_df.to_csv("oscillations/data/driven_force_data.csv", float_format="%.2f", index=False, sep=";", decimal=".")

df = pd.read_csv("oscillations/data/driven_force_data.csv", delimiter=";")
for M in range(1, 3):
    for i in range(1, 18):
        voltage = df[f"{M}voltage{i}"].mean()
        print(f"Measurement {M}\tSample {i}\tVoltage {voltage:.2f}V")
    print("\n")

damping = pd.read_csv("oscillations/data/damping.csv", delimiter=";")
damping.columns = ["time", "angle1", "angle2"]
damping.to_csv("oscillations/data/damping.csv", sep=";", decimal=".", float_format="%.2f", index=False)
    
# damping1 = pd.read_csv("oscillations/data/damping1.csv", delimiter=";")
# damping2 = pd.read_csv("oscillations/data/damping2.csv", delimiter=";")

# damping1 = damping1.dropna()
# damping2 = damping2.dropna()

# min_time = min([df.shape[0] for df in [damping1, damping2]])
# damping = [df.iloc[:min_time] for df in [damping1, damping2]]

# damping = pd.concat(damping, axis=1)
# damping.columns = ["1time", "1angle", "2time", "2angle"]

# damping.to_csv("oscillations/data/damping.csv", sep=";", decimal=".", float_format="%.2f", index=False)
 
# simple_harmonics = pd.read_csv("oscillations/data/simple_harmonics.csv", delimiter=";")
# simple_harmonics.pop(simple_harmonics.columns[0])

# simple_harmonics.to_csv("oscillations/data/simple_harmonics.csv", sep=";", decimal=".", float_format="%.2f", index=False)

# DANGEROUS AREA BELOW
# DANGEROUS AREA BELOW
# DANGEROUS AREA BELOW
# DANGEROUS AREA BELOW
# DANGEROUS AREA BELOW

# M1 = "oscillations/data/M1"
# M2 = "oscillations/data/M2"
# files = {
#     f"{M1}/2.csv": range(1, 3),
#     f"{M1}/3.csv": (1, 4),
#     f"{M1}/4.csv": (1, 5),
#     f"{M1}/5.csv": (1, 6),
#     f"{M1}/6.csv": (1, 7),
#     f"{M1}/7.csv": (1, 8),
#     f"{M1}/8.csv": (1, 9),
#     f"{M1}/9.csv": (1, 10),
#     f"{M1}/10.csv": (1, 11),
#     f"{M1}/11.csv": (1, 12),
#     f"{M1}/12.csv": (1, 13),
#     f"{M1}/13.csv": (1, 14),
#     f"{M1}/14.csv": (1, 15),
#     f"{M1}/15.csv": (1, 16),
#     f"{M1}/16.csv": (1, 17),
#     f"{M1}/17.csv": (1, 18),

#     f"{M2}/1.csv": (1, 20),
#     f"{M2}/2.csv": (1, 21),
#     f"{M2}/3.csv": (1, 22),
#     f"{M2}/4.csv": (1, 23),
#     f"{M2}/5.csv": (1, 24),
#     f"{M2}/6.csv": (1, 25),
#     f"{M2}/7.csv": (1, 26),
#     f"{M2}/8.csv": (1, 28),
#     f"{M2}/9.csv": (1, 27),
#     f"{M2}/11.csv": (1, 2),
#     f"{M2}/12.csv": (1, 3),
#     f"{M2}/13.csv": (1, 4),
#     f"{M2}/14.csv": (1, 5),
#     f"{M2}/15.csv": (1, 6),
#     f"{M2}/16.csv": (1, 7),
#     f"{M2}/17.csv": (1, 19)
# }

# def getcol(i: int):
#     return [
#         f"Angular Velocity, Ch 1+2 (rad/s) Run #{i}",
#         f"Angular Acceleration, Ch 1+2 (rad/s²) Run #{i}",
#         f"Position, Ch 1+2 (m) Run #{i}",
#         f"Velocity, Ch 1+2 (m/s) Run #{i}",
#         f"Acceleration, Ch 1+2 (m/s²) Run #{i}",
#         f"Angle, Ch 3+4 (rad) Run #{i}",
#         f"Angular Velocity, Ch 3+4 (rad/s) Run #{i}",
#         f"Angular Acceleration, Ch 3+4 (rad/s²) Run #{i}",
#        f"Position, Ch 3+4 (m) Run #{i}",
#        f"Velocity, Ch 3+4 (m/s) Run #{i}",
#        f"Acceleration, Ch 3+4 (m/s²) Run #{i}",
#        f"Output Voltage (V) Run #{i}",
#        f"Output Current (A) Run #{i}"
#    ]

    
# df = pd.read_csv("oscillations/data/simple_harmonics.csv", delimiter=";")
# # for i in range(2, 3):
# #    cols = getcol(i)
# #    for col in cols:
# #        df.pop(col)
# cols = ["angular_velocity", "angular_acceleration", "position", "velocity", "acceleration", "driven_angle", "driven_angular_velocity", "driven_angular_acceleration", "driven_position", "driven_velocity", "driven_acceleration", "voltage", "current"]
# for col in cols:
#     df.pop(col)
# df.columns = ["time", "angle"]
# df["time"] = df["time"].str.replace(",", ".")
# df["angle"] = df["angle"].str.replace(",", ".")
# df.to_csv(f"oscillations/data/simple_harmonics.new.csv", float_format="%.2f", decimal=".", sep=";", index=False)
 
