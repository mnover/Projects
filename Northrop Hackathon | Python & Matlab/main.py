# this code creates a window so users are able to input data.

# packages
from tkinter import *
from tkinter.ttk import *
import matlab.engine

# start up matlab
eng = matlab.engine.start_matlab()
s = eng.genpath("C:/") # removed for privacy
eng.addpath(s, nargout=0)


# functions
def update_UAV(): # updates total amount of UAVs
    global totalUAVs
    totalUAVs = int(UAVSpinbox.get())

def send_ToMatLab(): # updates all position related info + sends to matlab to run
    global positions

    i = 0
    for i in range(0, 4):
        val = positions["Shape_corners"][i]
        if val["Lat_text"].get() != "":
            try:
                positions["Shape_corners"][i]["xy"][0] = float(val["Lat_text"].get())
            except:
                print("")

        if val["Long_text"].get() != "":
            try:
                positions["Shape_corners"][i]["xy"][1] = float(val["Long_text"].get())
            except:
                print("")

    # preparing data to be used in matlab
    positions["UAVsp"][0] = float(latitude_Entry.get())
    positions["UAVsp"][1] = float(longitude_Entry.get())

    shapeBounds = [corner["xy"] for corner in positions["Shape_corners"]]
    shapeBounds_matlab = matlab.double(shapeBounds)

    # matlab call function:
    #([long lat],[long lat; long lat; long lat; long lat], UAV #, maxspeed, height, sensor coverage)
    eng.main(matlab.double(positions["UAVsp"]), shapeBounds_matlab, float(totalUAVs), float(UAVspecs["Speed"][0]),
             float(UAVspecs["Height"][0]), float(UAVspecs["Search Width"][0]))

def save_DroneSpec(): # updates drone specs
    global UAVspecs

    for i in UAVspecs:
        UAVspecs[i][0] = UAVspecs[i][1].get()

def edit_UAVspecs(): # opens drone spec window
    global engaged

    if not engaged:
        engaged = True
        UAVwindow = Tk()

        # important close function
        def closing_UAV():
            global engaged

            save_DroneSpec()
            UAVwindow.destroy()
            engaged = False

        # window adjustments
        UAVwindow.title("UAV specs")
        UAVwindow.geometry("400x250-550+300")

        # title creation
        UAV_titleText = Label(UAVwindow, text = "UAV Specifications", font = ("Arial", 20))
        UAV_titleText.place(relx = .5, rely = .13, anchor = "center")

        # all text details
        i = 0; j = 0
        for i in UAVspecs:
            newy = .3 + (j * 0.15)

            textDesc = Label(UAVwindow, text = i + ":", font = ("Arial", 12), justify = "right")
            textDesc.place(relx = 0.15, rely = newy, anchor = "center")
            textEntry = Entry(UAVwindow, font = ("Arial", 12), justify = "center")
            textEntry.insert(END, str(UAVspecs[i][0]))
            textEntry.place(relx = 0.55, rely = newy, anchor = "center")

            UAVspecs[i][1] = textEntry
            j += 1

        # button creation
        saveButton = Button(UAVwindow, text = "Save", command = save_DroneSpec)
        saveButton.place(relx = .5, rely = .9, anchor = "center")

        UAVwindow.protocol("WM_DELETE_WINDOW", closing_UAV)

# closing program
def close_mainWindow():
    eng.quit()

# important variables
window = Tk() # the window everything appears on
totalUAVs = 1 # how many UAVs will be used
engaged = False # window checker
positions = { # important coordinates: [Latitude, Longitude]
    "UAVsp": [0, 0, 0],

    "Shape_corners": [
        {"xy": [0, 0],
         "Lat_text": None,
         "Long_text": None},
        {"xy": [0, 0],
         "Lat_text": None,
         "Long_text": None},
        {"xy": [0, 0],
         "Lat_text": None,
         "Long_text": None},
        {"xy": [0, 0],
         "Lat_text": None,
         "Long_text": None},
    ]
}
UAVspecs = { # important UAV specs
    "Height": [25, None], # in meters
    "Speed": [10, None], # m/s
    "Search Width": [10, None], # in meters
    "Battery Life": [200, None] # in minutes
}

# window adjustments
window.title("UAV Search and Rescue")
window.geometry("600x500-450+200")

# title creation
titleText = Label(window, text = "UAV Path Generator", font = ("Arial", 20))
titleText.place(relx = .5, rely = .05, anchor = "center")

# gathering information portion:
# text and spinbox for number of UAV
num_UAVText = Label(window, text = "Number of UAVs:", font = ("Arial", 12))
num_UAVText.place(relx = 0.43, rely = 0.15, anchor = "center")
UAVSpinbox = Spinbox(window, from_ = 1, to = 6, width = 6, command = update_UAV, font = ("Arial", 12))
UAVSpinbox.place(relx = 0.6, rely = 0.15, anchor = "center")
UAVSpinbox.set(1)

# spawn Point longitude, latitude text and Entry box
coordinateText = Label(window, text = "UAV Starting Point:", font = ("Arial", 15))
coordinateText.place(relx = 0.21, rely = 0.25, anchor = "center")
latitudeText = Label(window, text = "Latitude", font = ("Arial", 12))
latitudeText.place(relx = 0.4, rely = 0.32, anchor = "center")
longitudeText = Label(window, text = "Longitude", font = ("Arial", 12))
longitudeText.place(relx = 0.75, rely = 0.32, anchor = "center")

latitude_Entry = Entry(window, font = ("Arial", 12), justify = "center")
latitude_Entry.place(relx = 0.4, rely = 0.37, anchor = "center")
longitude_Entry = Entry(window, font = ("Arial", 12), justify = "center")
longitude_Entry.place(relx = 0.75, rely = 0.37, anchor = "center")

# Bound box longitude, latitude text and Entry box
shapeTitleText = Label(window, text = "Search Area Bounds: ", font = ("Arial", 15))
shapeTitleText.place(relx = 0.23, rely = 0.48, anchor = "center")
latitudeDescText = Label(window, text = "Latitude", font = ("Arial", 12))
latitudeDescText.place(relx = 0.4, rely = 0.57, anchor = "center")
longitudeDescText = Label(window, text = "Longitude", font = ("Arial", 12))
longitudeDescText.place(relx = 0.75, rely = 0.57, anchor = "center")
i = 0

for i in range(0, 4):
    newy = .62 + (i * 0.07)

    CordText = Label(window, text = "Coordinate "+str(i + 1)+":", font = ("Arial", 12))
    CordText.place(relx = 0.15, rely = newy, anchor = "center")
    lo_latitude_Entry = Entry(window, font = ("Arial", 12), justify = "center")
    lo_latitude_Entry.place(relx = 0.4, rely = newy, anchor = "center")
    hi_longitude_Entry = Entry(window, font = ("Arial", 12), justify = "center")
    hi_longitude_Entry.place(relx = 0.75, rely = newy, anchor = "center")

    positions["Shape_corners"][i]["Lat_text"] = lo_latitude_Entry
    positions["Shape_corners"][i]["Long_text"] = hi_longitude_Entry

# create button
submitButton = Button(window, text = "Generate Path", command = send_ToMatLab)
submitButton.place(relx = .5, rely = .9, anchor = "center")


# frame options
topBar = Menu(window)
UAVInfo = Menu(topBar, tearoff = 0)
topBar.add_cascade(label = "UAV Criteria", menu = UAVInfo)
UAVInfo.add_command(label = "UAV Specifications", command = edit_UAVspecs)
UAVInfo.add_command(label = "Exit", command = window.destroy)

# display Menu
window.config(menu = topBar)
window.mainloop()

window.protocol("WM_DELETE_WINDOW", close_mainWindow)
