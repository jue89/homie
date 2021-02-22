$fn = 50;

module body (w, h, d) {
    linear_extrude (height = d, center = true) hull () {
            translate([w / 2 - h / 2, 0, 0]) circle(d = h);
            translate([w / -2 + h / 2, 0, 0]) circle(d = h);
    };
}

module half (h, w, d, wall) {
    union() {
        difference () {
            // Outer shell
            body(w = w + 2 * wall, h = h + 2 * wall, d = d + 2 * wall);
            
            // Inner body
            translate([0, 0, -wall / 2])
            body(w = w, h = h, d = d + 2 * wall);
            
            // Cut in two
            translate([0, (h + wall) / 2, -wall / 2])
                cube([w + h + 5 * wall, h + wall, d + 2 * wall], center = true);
        };
        
        // Housing lip
        translate([0, 0, d / 2 + wall / 2])
            body(w = w, h = h, d = wall);
    };
}

N = 1; // Button Count = N * 2 + 1
pitch = 26; // Distance between two buttons
wall = 2; // Wall thinksness
height = 28; // Inner height
depth = 40; // Inner depth
width = 2 * N * pitch + height; // Inner widht
lpheight = 1.65; // Height of PCB
screws = [ // Screw positions measured from housing center
    [-pitch/2, -depth/2 - wall + 23 + 10.67],
    [pitch/2, -depth/2 - wall + 23 + 10.67]
];
screwh1 = (height - 16.68) / 2 - 1 - lpheight; // Hight of dome 1
screwh2 = height - screwh1 - lpheight; // Hight of dome 2
screwhole = 3.2; // Diameter of screw hole
screwheadd = 5.6; // Diameter of screw head
screwheadh = 2.2; // Height of screw head
ruthexd=4; // Ruthex diameter
ruthexh=7; // Ruthex height
switchd=16.3; // Switch diameter
ifaceh=17; // Bus interface height
ifacew=12; // Bus interface width

module front () {
    union() {
        difference () {
            // Housing half
            half(w = width, h = height, d = depth, wall = wall);
            
            // - Breakout for switches
            for (i = [-N : N]) {
                translate([i * pitch, 0, 0])
                    cylinder(h = depth, d = switchd);
            }
        }
        for (p = screws) {
            translate([0, screwh2 - height / 2, 0]) difference() {
                // Dome
                translate([p[0], 0, p[1]]) rotate([90, 0, 0]) cylinder(h=screwh2, d=8);
                
                // - Ruthex pocket
                translate([p[0], 1, p[1]]) rotate([90, 0, 0]) cylinder(h=ruthexh+1, d=ruthexd);
                
                // - Screw hole
                translate([p[0], 2, p[1]]) rotate([90, 0, 0]) cylinder(h=screwh2+1, d=screwhole);
            }
        };
    }
};

module back () {
    difference() {
        union() {
            difference () {
                // Body
                rotate([0, 180, 180]) half(w = width, h = height, d = depth, wall = wall);
                
                // - Bus ports
                translate([-pitch / 2, 0, -depth / 2]) cube([ifacew, ifaceh, depth], center = true);
                translate([pitch / 2, 0, -depth / 2]) cube([ifacew, ifaceh, depth], center = true);
            };
            
            // Domes
            for (p = screws) {
                translate([p[0], height / 2, p[1]]) rotate([90, 0, 0]) cylinder(h=screwh1, d=8);
            };
        };
        
        // - Holes
        for (p = screws) {
            // Screw
            translate([p[0], height, p[1]]) rotate([90, 0, 0]) cylinder(h = height, d = screwhole);
            // Screw head
            translate([p[0], height/2 + 2 * wall, p[1]]) rotate([90, 0, 0]) cylinder(h = screwheadh + wall, d = screwheadd);
        };
    }
}

front();
back();