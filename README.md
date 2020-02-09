# esp32smartdoorbell-chime

Like most of us, im a bit time poor, so sorry for the lack of info!

Essentially this uses:
2 esp32's - one with a camera, one without

I started with the 10 dollar webcam article and added a button. When the button is pressed, a get request is pushed to the second esp32. 
The second esp32 then plays a tone through a 50mm speaker direct from the esp32 using a tone library.

The setup works very reliably. Both devices use the esp32 autowifi library which makes setup for wifi easier (there are some docs online for that)

THings this that could be improved:
Dicoevery of master / slave to avoid manually mathcing the I.P. address of the devices. 
The webcam end needs to know the I.P. address of the chime.

The tone could be improved... anybody?

Hope you enjoy! if you need any info just let me know!
