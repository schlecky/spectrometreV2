// Spectrometre avec capteur CCD TCD1304
kerf=0.2;
e=3;
//Décalage de la boite suivant y
dyB = -40;
$fs=0.1;
$fa=6;

// Intérieur de la boite
hIntBoite = 62.5;
LIntBoite=178;
lIntBoite=132;

module objectifNeotar(){
    rotate([0,90,0])
    translate([0,0,-21.5/2])
    color("grey")
    union(){
        cylinder(h=21.5, r=38.9/2, center=true);
        translate([0,0,(18-2.5)/2]) cylinder(h=2.5, r=41.8/2, center=true);
    }
}

module objectifSiS(){
    rotate([0,90,0])
    translate([0,0,-57.4/2]){  
        color("grey")union(){
            cylinder(h=57.4, r=49.7/2, center=true);
            translate([0,0,(57.4-6.5)/2]) cylinder(h=6.5, r=57/2, center=true);
            translate([0,0,-(57.4+15)/2]) cylinder(h=15, r=30/2, center=true);
            translate([0,0,-(57.4+2*15+4)/2]) cylinder(h=4, r=25/2, center=true);

        }
        // Support réseau
        color("white")translate([0,0,(57.41+4)/2])difference(){
            cylinder(h=4, r=56.5/2, center=true);
            cylinder(h=4.5, r=44/2, center=true);
        }
    }
}

module ccd(){
    translate([9,-(41.5/2+13),-13])
    rotate([0,-90,0]){
        color("white")
        difference(){
            cube([26,63,1.5]);
            translate([2,5,-0.1])cylinder(h=3,r=2.5/2);
        }
        // uspport CI
        color("blue")translate([6.5,18,1.5])cube([12.5,30,5]);
        translate([5,60,1.5])color("black")cylinder(r=6.5/2,h=13);
        
        translate([8,13,6.5]){
            color("brown")difference(){
                cube([10,41.5,4]);
                translate([2.5,2,3.5])cube([5,37,2]);
            }
            color("black")translate([4.5, 6.25, 2.8])cube([1,29,1]);
        }
    }
}

module boite(){
    L=187.5;
    l=139;
    h=68;
    eBoite=2;
    difference(){
        cube([L,l,h]);
        translate([eBoite,eBoite,eBoite])cube([L-2*eBoite, l-2*eBoite, h]);
    } 
}



module ecran(){
    h = hIntBoite-2*e;
    l=66.5;
    color("gray")
    translate([28,-35,-h/2])
    union(){
        cube([l, e, h]);
        translate([6,0,-3])cube([6,e,h+6]);
        translate([l-2*6,0,-3])cube([6,e,h+6]);
    }  
}


module circuit(){
    translate([-24,-30,-27]){
        color("khaki")cube([1.5,100,54]);
        color("red")translate([13,6.3,1])cube([10,66,51]);
        color("gray")translate([0,73,21])cube([27.5,13,11]);
    }
}

module supportCircuit(){
    h=hIntBoite-2*e;
    translate([-28,dyB+1.5+3,-h/2])
    union(){
        cube([e,126,h]);
        translate([0,6,-3])cube([e, 6, h+6]); 
        translate([0,120-6,-3])cube([e, 6, h+6]);
        translate([0,126/2-6,-3])cube([e, 6, h+6]); 
    }
}


ev = 38;    // entre-axes vertical
eh=124;     // entre-axes horizontal
module faceAvant(){ 
    h=69;
    l=134.7;
    echo((l-eh)/2);
    difference(){
        translate([0,dyB,-h/2])cube([e, l, h]);
        translate([50,0,0])objectifSiS();
        
        // Trous
        translate([-e/2,dyB+(l-eh)/2,-(ev)/2])rotate([0,90,0])cylinder(d=3.5, h=2*e);
        translate([-e/2,dyB+(l+eh)/2,-(ev)/2])rotate([0,90,0])cylinder(d=3.5, h=2*e);
        translate([-e/2,dyB+(l-eh)/2,(ev)/2])rotate([0,90,0])cylinder(d=3.5, h=2*e);
        translate([-e/2,dyB+(l+eh)/2,(ev)/2])rotate([0,90,0])cylinder(d=3.5, h=2*e);
    } 
}

module faceArriere(){ 
    h=69;
    l=134.7;
    color("brown")
    difference(){
        translate([0,dyB,-h/2])cube([e, l, h]);
        circuit();
        
        // Trous
        translate([-e/2,dyB+(l-eh)/2,-(ev)/2])rotate([0,90,0])cylinder(d=3.5, h=2*e);
        translate([-e/2,dyB+(l+eh)/2,-(ev)/2])rotate([0,90,0])cylinder(d=3.5, h=2*e);
        translate([-e/2,dyB+(l-eh)/2,(ev)/2])rotate([0,90,0])cylinder(d=3.5, h=2*e);
        translate([-e/2,dyB+(l+eh)/2,(ev)/2])rotate([0,90,0])cylinder(d=3.5, h=2*e);
    }
} 



module supportSiS(){
    l=62;               // Largeur du support
    h=hIntBoite-2*e;    // hauteur du support
    epSupport = 15;     // Épaisseur du support
    module plaqueVert(){
        union(){
            difference(){
                translate([0,-l/2,-h/2])cube([e,l,h]);
                translate([50,0,0])objectifSiS();
            }
            translate([0,-l/2,h/2-0.5])cube([e,6,e+0.5]);
            translate([0,-l/2,-h/2-e])cube([e,6,e+0.5]);
            translate([0,l/2-6,-h/2-e])cube([e,6,e+0.5]);
            translate([0,l/2-6,h/2-0.5])cube([e,6,e+0.5]);
        }
    }
    module plaqueVert2(){
        union(){
            difference(){
                translate([0,-l/2,-h/2])cube([e,l,h]);
                translate([50,0,0])objectifSiS();
            }
            translate([0,-l/2+12,h/2-0.5])cube([e,6,e+0.5]);
            translate([0,l/2-18,h/2-0.5])cube([e,6,e+0.5]);
            translate([0,l/2-18,-h/2-e])cube([e,6,e+0.5]);
            translate([0,-l/2+12,-h/2-e])cube([e,6,e+0.5]);
        }
    }
    module separateur(){
        cube([epSupport-2*e, e, h]);
    }
    translate([0,0,0])plaqueVert();
    translate([e,0,0])plaqueVert2();
    translate([2*e,0,0])plaqueVert();
    translate([3*e,0,0])plaqueVert2();
    translate([4*e,0,0])plaqueVert();
    //translate([e,-l/2,-h/2])separateur();
    //translate([e,l/2-e,-h/2])separateur();
}

module supportNeotar(){
    ls=60;              // Largeur du support
    lc=110;             // Largeur du support
    h=hIntBoite-2*e;    // hauteur du support
    //epSupport = ;     // Épaisseur du support
    module plaqueVert(l){
        union(){
            difference(){
                translate([-21,-l/2,-h/2])cube([e,l,h]);
                translate([0,0,0])objectifNeotar();
            }
            translate([-21,-l/2,-h/2-e])cube([e,6,e+0.5]);
            translate([-21,l/2-6,-h/2-e])cube([e,6,e+0.5]);
            translate([-21,-3-10,-h/2-e])cube([e,6,e+0.5]);
            translate([-21,-3+10,-h/2-e])cube([e,6,e+0.5]);
            translate([-21,-l/2,h/2-0.5])cube([e,6,e+0.5]);
            translate([-21,l/2-6,h/2-0.5])cube([e,6,e+0.5]);
            translate([-21,-3-10,h/2-0.5])cube([e,6,e+0.5]);
            translate([-21,-3+10,h/2-0.5])cube([e,6,e+0.5]);
        }
    }
    //plaqueVert();
    translate([e,0,0])plaqueVert(lc);
    translate([2*e,0,0])plaqueVert(ls);
    translate([3*e,0,0])plaqueVert(ls);
}

module supportCCD(){
    lP1=70;   // largeur plaque1
    lP2= 50; // largeur de la plaque2
    h=hIntBoite-2*e; //hauteur totale
    hP1 = 40; // hauteur de la plaque 1 fixée sur le circuit
    module plaque1(){
        translate([9-e-1.5,-lP1+17,-hP1/2]){
            difference(){
                cube([e,lP1,hP1]);
                translate([-e/2,10,6])rotate([0,90,0])cylinder(h=2*e, r=1.5);
                translate([-e/2,10,hP1-6])rotate([0,90,0])cylinder(h=2*e, r=1.5);
                translate([-e/2,35,hP1/2-13/2])cube([2*e,50,13]);
           }
        }
    }
    module plaque2(){
        translate([9-2*e-1.5,-lP2-22,-h/2]){
            union(){
                difference(){
                    cube([e, lP2, h]);
                    translate([0,27.5,h/2-hP1/2+3])cube([2*e, 3, 6]);
                    translate([0,27.5,h/2+hP1/2-9])cube([2*e, 3, 6]);
                }
                translate([0,6,-3])cube([e,6,h+2*e]);
                translate([0,lP2-2*6,-3])cube([e,6,h+2*e]);
            }
        }
    }
    plaque1();
    plaque2();
}




module interieur(){
    dx=30;
    translate([dx,0,0]){
        objectifSiS();
        color("blue")translate([-dx+5,0,0])supportSiS();
        dxN=43;
        rotate([0,0,38.4]){
            translate([dxN,-0,0]){
                objectifNeotar();
                color("red")supportNeotar();
            }
            translate([dxN+50,0,0]){
            ccd();
            color("gray")supportCCD();
            }
            ecran();
            translate([0,67,0])ecran();
        }
    }
    translate([LIntBoite+e,0,0]){
        supportCircuit();
    }
}



module plaqueDessous(){
    difference(){
        translate([e,dyB+1.5,-hIntBoite/2])cube([LIntBoite,lIntBoite, e]);
        interieur();
    }
}
module plaqueDessus(){
    difference(){
        translate([e,dyB+1.5,hIntBoite/2-e])cube([LIntBoite,lIntBoite, e]);
        interieur();
    }
}

module assemblage(){
    interieur();
    color("brown")faceAvant();
    translate([LIntBoite+e,0,0]){
        faceArriere();
        circuit();
    }
    plaqueDessous();
    //plaqueDessus();
}


module plan(){

    projection(cut=true){
        //face avant
        translate([0,0,e/2])rotate([0,90,0])faceAvant();
        //face arriere
        translate([0,140,e/2])rotate([0,90,0])faceArriere();
        //Plaque dessous
        translate([40,0,hIntBoite/2-e/2])plaqueDessous();
        //Plaque dessus
        translate([40,140,-hIntBoite/2+e/2])plaqueDessus();
        //Support SiS
        for(i=[0:4])
        translate([i*65,270,e/2+e*i])rotate([0,90,0])supportSiS();
        //support CCD
        translate([260,200,e])rotate([0,90,0])supportCCD();
        translate([260,100,2*e])rotate([0,90,0])supportCCD();
        //Support neotar
        translate([340,0,-3.5*e])rotate([0,90,0])supportNeotar();
        translate([340,70,-4.5*e])rotate([0,90,0])supportNeotar();
        translate([340,160,-5.5*e])rotate([0,90,0])supportNeotar();
        //Support circuit
        translate([400,250,0])rotate([0,90,90])translate([28-e/2,0,0])supportCircuit();
        //ecran
        translate([230,-10,0])rotate([90,0,0])translate([-28,35-e/2,0])ecran();
        translate([380,-10,0])rotate([90,0,0])translate([-28,35-e/2,0])ecran();
}
    
    

    
    
}


//plan();
assemblage();

//objectifNeotar();
//plaqueDessous();
//ccd();
//supportCCD();
//faceArriere();
//supportCircuit();