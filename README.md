## IMS Project
Author: Jakub Sychra 
Year 2023

Cellular Automata in Traffic

### Description

This project analyses and verifies solutions to an increasing traffic problem on a specific junction studied in a thesis. (more in *documentation.pdf*).

### Program usage

./model -a <b>\<A></b> -b <b>\<B></b> -c <b>\<C></b> -d <b>\<D></b> -e <b>\<E></b> -f <b>\<F></b> -g <b>\<G></b> -h <b>\<H></b> -i <b>\<I></b> -p <b>\<P></b>

* where <b>\<A></b> is a number of cars measured on point A 
* where <b>\<B></b> is a number of cars measured on point B 
* where <b>\<C></b> is a number of cars measured on point C 
* where <b>\<D></b> is a number of cars measured on point D 
* where <b>\<E></b> is a number of cars measured on point E 
* where <b>\<F></b> is a number of cars measured on point F 
* where <b>\<G></b> is a number of cars measured on point G 
* where <b>\<H></b> is a number of cars measured on point H 
* where <b>\<I></b> is a number of iterations in minutes
* where <b>\<P></b> is a toggle for extended road network option

The car measurements are determined from the original junction study and are used as probabilites of car creation at certain junction points