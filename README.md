# ECG_PPG-DataLogger
The SerialData Logger for the Attiny85_ECG_PPG project.Written in C++ since Processing/Java was slower and couldn't handle high speed data

# Experiece with Processing 2
was using Processing to Log and plot data but that parsing was too noisy and was not able to handle the high speed data probably because it was plotting the data simultaneously on the window as well.

Instead of rectifying it, which i could obviously using the same algo i used in c++ , but i didnt because maybe even if i used the same algo it might still be slower and therefore noisy so,i thought of quickly writing a C++ application to do just the same since i would be sure that it would be the fastest that i can go.

Thats what i did and it took only 2 work days .YAY!

# FLOW
- On running the application it asks for the COM port at which the data is to be read from 
- the com port can be entered for example ( com9 or com10 or COM7 )
- then it checks if we are connected successfully to the port
- then the User can choose from S/P/E (Start,Pause,EXIT respectively) to control the application
- At any point during the application runtime Ctrl+C can be pressed to pause the data Collection and S/P/E to (Start,Pause,EXIT) respectively
