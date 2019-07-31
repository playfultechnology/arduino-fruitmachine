ledHolderScrewType2();


module ledHolderScrewType2() {

difference() {
  union() {
      /*
      translate([22.2/2, -4, -10.3/2])
      rotate([90,0,0])
      #intersection() {
          cylinder(r=18/2, h=4);
          translate([-10/2, -18/2, 0])
            cube([10,18,12]);
        }
      */
      
    // Plate with clips
    linear_extrude(height=2)
      polygon(points = [[0-3,0-3],[22.2+3,-3],[22.2+3,10.3+3], [22.2-3,10.3+3], [3,3], [0-3,3]]);
    translate([-0,0,-2])
      cylinder(r=1.4, h=2.4, $fn=100);
    translate([22.2,10.3,-2])
      cylinder(r=1.4, h=2.4, $fn=100); 
    // Blank plate
    translate([22.2/2-3,-3,-10.3-2])
      cube([6,12,2]);
  }
  // Side channel for feeding wires to LED
  translate([22.2/2-3.3, -4, -10.3/2-1])
    cube([6.6,3,20]);
}
// Main block
difference() {
    
  union(){
    translate([22.2/2, -3, -10.3/2])
      rotate([90,0,0])
        intersection() {
          cylinder(r=18.2/2, h=6.8);
          translate([-10/2, -18.2/2, 0])
            cube([10,18,12]);
        }
        translate([22.2/2-6, -4.8, -10.3/2-14])
        intersection() {
            #cube([12,2,28]);
         rotate([90,0,0])
            translate([6, 14, -2])
        #cylinder(r=12, h=2);
        }
  }

  // Twist to secure channel
  #translate([11.5,-9.8,-14.3])
  rotate([0,0,0])
    linear_extrude(height=2.4)
    polygon(points = [[-.4,0],[3,0],[3,5],[-4.2,5], [-4.2,2.5], [-.4,2.5] ]);

  // Twist to secure other channel
  #translate([11,-9.8,4.0])
  rotate([0,180,0])
    linear_extrude(height=2.4)
    polygon(points = [[-.4,0],[3,0],[3,5],[-4.2,5], [-4.2,2.5], [-.4,2.5] ]);
  
  // Channel for LED legs
  translate([22.2/2, 10, -10.3/2])
    rotate([90,0,0])
      cylinder(r=2.5, h=20, $fn=100);
  
    translate([22.2/2-3.3, -31, -10.3/2-1])
      #cube([6.6,50,2]);
  
  
  // Side channel for feeding wires to LED
  translate([22.2/2-3.3, -5.2, -10.3/2])
    #cube([6.6,3,10.3/2+2]);
}    

// LED holder
difference() {
translate([22.2/2, -8, -10.3/2])
  rotate([90,0,0], $fn=100)
    cylinder(r=4.5, h=10);

  translate([22.2/2, -15.1, -10.3/2])
    rotate([90,0,0])
      #cylinder(r=3, h=10, $fn=100); 
  
  translate([22.2/2, 0, -10.3/2])
   rotate([90,0,0])
    cylinder(r=2.5, h=18, $fn=100);

  translate([22.2/2-3.3, -31, -10.3/2-1])
      #cube([6.6,50,2]);

}

}




module ledHolderScrewType() {

difference() {
  union() {
    // Plate with clips
    linear_extrude(height=2)
      polygon(points = [[0-3,0-3],[22.2+3,-3],[22.2+3,10.3+3], [22.2-3,10.3+3], [3,3], [0-3,3]]);
    translate([-0,0,-2])
      cylinder(r=1.4, h=2.4, $fn=100);
    translate([22.2,10.3,-2])
      cylinder(r=1.4, h=2.4, $fn=100); 
    // Blank plate
    translate([22.2/2-3,-3,-10.3-2])
      cube([6,12,2]);
  }
  // Channel for LED legs
  translate([22.2/2-2.8, -3, -10.3/2-1])
    #cube([5.6,2,20]);
}
// Main block
difference() {
    
  union(){
    translate([22.2/2, -3, -10.3/2])
      rotate([90,0,0])
        intersection() {
          cylinder(r=18/2, h=5.6);
          translate([-10/2, -18/2, 0])
            cube([10,18,12]);
        }
  }

  // Twist to secure channel
  translate([11.5,-8.6,-14.2])
  rotate([0,0,0])
    linear_extrude(height=2.4)
    polygon(points = [[0,0],[3,0],[3,4.8],[-4.2,4.8], [-4.2,3], [0,3] ]);

  // Twist to secure other channel
  translate([11,-8.6,3.9])
  rotate([0,180,0])
    linear_extrude(height=2.4)
    polygon(points = [[0,0],[3,0],[3,4.8],[-4.2,4.8], [-4.2,3], [0,3] ]);
  
  translate([22.2/2-2.8, -3, -10.3/2-2.8])
    #cube([5.6,4,24]);
  
  // Channel for LED legs
  translate([22.2/2, -5, -10.3/2])
    rotate([90,0,0])
      cylinder(r=2.5, h=20, $fn=100);
  
  
}    

// LED holder
difference() {
translate([22.2/2, -8, -10.3/2])
  rotate([90,0,0], $fn=100)
    cylinder(r=4.5, h=10);

  translate([22.2/2, -15.1, -10.3/2])
    rotate([90,0,0])
      cylinder(r=3, h=3, $fn=100); 
  
  translate([22.2/2, -8, -10.3/2])
   rotate([90,0,0])
    #cylinder(r=2.5, h=18, $fn=100);
      //#cube([5.6,50,2]);

}
}







module ledHolderPushType() {

// Plate with clips
linear_extrude(height=2)
  polygon(points = [[0-3,0-3],[22.2+3,-3],[22.2+3,10.3+3], [22.2-3,10.3+3], [3,3], [0-3,3]]);
translate([-0,0,-2])
  cylinder(r=1.4, h=2.4, $fn=100);
translate([22.2,10.3,-2])
  cylinder(r=1.4, h=2.4, $fn=100); 
// Blank plate
translate([22.2/2-3,-3,-10.3-2])
  cube([6,12,2]);




// Main block
difference() {
    
  union(){
    translate([22.2/2, -3, -10.3/2])
      rotate([90,0,0])
        intersection() {
          cylinder(r=18/2, h=10);
          translate([-10/2, -18/2, 0])
            cube([10,18,12]);
        }

    // Pins
    translate([22.2/2, -2.8 -8.6, -21/2-10.3/2])
      cylinder(r=2.1/2, h=21, $fn=100);
  }

  // Flex holes
  translate([22.2/2-6, -15, -.5])
    cube([12,10,2]);
  translate([22.2/2-6, -15, -11.9])
    cube([12,10,2]);
  
  // Side channel for LED legs
  translate([22.2/2-3.3, -7, -10.3/2])
  #cube([6.6,4,12]);
  
  // Central channel for LED legs
  translate([22.2/2-3.3, -25, -10.3/2-1])
    cube([6.6,50,2]);

  translate([22.2/2, 0, -10.3/2])
    rotate([90,0,0])
      #cylinder(r=2.5, h=30, $fn=100);   
  
}    

// LED holder
difference() {
translate([22.2/2, -12, -10.3/2])
  rotate([90,0,0], $fn=100)
    cylinder(r=4.5, h=16);

  translate([22.2/2, -25.1, -10.3/2])
    rotate([90,0,0])
      cylinder(r=3.5, h=3, $fn=100); 
  
  translate([22.2/2-3.3, -31, -10.3/2-1])
      cube([6.6,50,2]);

  translate([22.2/2, 0, -10.3/2])
    rotate([90,0,0])
      #cylinder(r=2.5, h=30, $fn=100);   
  

}
}

//translate([0,0,-10.3])
//v3_micro_switch(type = 1); // 1 = std switch, 2 = long roller, 3 = short roller, 4 = lever







//Outer module - sets parameters
module v3_micro_switch(type = 2)
{

if(type == 1) // 
{
	_micro_switch(
				ms_length= 22.2,
				ms_height = 10.3,
				ms_width = 10.3,
				ms_m_hole_rad = 3.1 / 2,
				ms_m_hole_offset = (3.4 - 3.1)/2,
				ms_outer_rad = 2.8,
				ms_switch_pos = 20.1,
				ms_switch_rad = 2,
				ms_switch_width = 4.2,
				ms_switch_length = 2.8,
				ms_switch_pt = 1.2,
				ms_switch_ot = 0.8,
				ms_open= true, // 0 = open or 1 = closed
				
				ms_term_width = 4.8,
				ms_term_length = 10,
				ms_term_thick = 0.6,
				ms_term_offset_1_y = 2.7,
				ms_term_offset_2_y = 0.7,
				ms_term_offset_3_y = 7,	

				ms_roller = false, 
				ms_roller_rad = 0,
				ms_roller_width = 0,
				ms_roller_offset = 0,
				ms_roller_length = 0,
				ms_roller_thick = 0,
				ms_roller_clip = 0
				
				
			);
}
if(type == 2) //long roller
{
	_micro_switch(
				ms_length= 22.2,
				ms_height = 10.3,
				ms_width = 10.3,
				ms_m_hole_rad = 3.1 / 2,
				ms_m_hole_offset = (3.4 - 3.1)/2,
				ms_outer_rad = 2.8,
				ms_switch_pos = 20.1,
				ms_switch_rad = 2,
				ms_switch_width = 4.2,
				ms_switch_length = 2.8,
				ms_switch_pt = 1.2,
				ms_switch_ot = 0.8,
				ms_open= true, // 0 = open or 1 = closed
				
				ms_term_width = 4.8,
				ms_term_length = 10,
				ms_term_thick = 0.6,
				ms_term_offset_1_y = 2.7,
				ms_term_offset_2_y = 0.7,
				ms_term_offset_3_y = 7,	

				ms_roller = true, 
				ms_roller_rad = 4.8 /2,
				ms_roller_width = 4.8,
				ms_roller_offset = 8.5,
				ms_roller_length = 34,
				ms_roller_thick = 0.6,
				ms_roller_clip = 2
				
				
			);
}
if(type == 3) //short roller
{
	_micro_switch(
				ms_length= 22.2,
				ms_height = 10.3,
				ms_width = 10.3,
				ms_m_hole_rad = 3.1 / 2,
				ms_m_hole_offset = (3.4 - 3.1)/2,
				ms_outer_rad = 2.8,
				ms_switch_pos = 20.1,
				ms_switch_rad = 2,
				ms_switch_width = 4.2,
				ms_switch_length = 2.8,
				ms_switch_pt = 1.2,
				ms_switch_ot = 0.8,
				ms_open= true, // 0 = open or 1 = closed
				
				ms_term_width = 4.8,
				ms_term_length = 10,
				ms_term_thick = 0.6,
				ms_term_offset_1_y = 2.7,
				ms_term_offset_2_y = 0.7,
				ms_term_offset_3_y = 7,	

				ms_roller = true, 
				ms_roller_rad = 4.8 /2,
				ms_roller_width = 4.8,
				ms_roller_offset = 8.5,
				ms_roller_length = 20.1,
				ms_roller_thick = 0.6,
				ms_roller_clip = 2
				
				
			);
}

if(type == 4)//lever
{
	_micro_switch(
				ms_length= 22.2,
				ms_height = 10.3,
				ms_width = 10.3,
				ms_m_hole_rad = 3.1 / 2,
				ms_m_hole_offset = (3.4 - 3.1)/2,
				ms_outer_rad = 2.8,
				ms_switch_pos = 20.1,
				ms_switch_rad = 2,
				ms_switch_width = 4.2,
				ms_switch_length = 2.8,
				ms_switch_pt = 1.2,
				ms_switch_ot = 0.8,
				ms_open= true, // 0 = open or 1 = closed
				
				ms_term_width = 4.8,
				ms_term_length = 10,
				ms_term_thick = 0.6,
				ms_term_offset_1_y = 2.7,
				ms_term_offset_2_y = 0.7,
				ms_term_offset_3_y = 7,	

				ms_roller = true, 
				ms_roller_rad =0,
				ms_roller_width = 4.8,
				ms_roller_offset = 8.5,
				ms_roller_length = 60,
				ms_roller_thick = 0.6,
				ms_roller_clip =0
				
				
			);
}

}

//inner module - calculates geometry
module _micro_switch(	
				ms_length,
				ms_height,
				ms_width,
				ms_m_hole_rad,
				ms_m_hole_offset,
				ms_outer_rad,
				ms_switch_pos,
				ms_switch_rad,
				ms_switch_width,
				ms_switch_length,
				ms_switch_pt,
				ms_switch_ot,
				ms_open,

				ms_term_width,
				ms_term_length,
				ms_term_thick,
				ms_term_offset_1_y,
				ms_term_offset_2_y,
				ms_term_offset_3_y,	

				ms_roller,
				ms_roller_rad,
				ms_roller_width,
				ms_roller_offset,
				ms_roller_length,
				ms_roller_thick,
				ms_roller_clip
			    )

{
ms_switch_len =  ms_switch_ot;

/*
if(ms_open == 1)	{ms_switch_len = ms_switch_ot;}
else(){ ms_switch_len = ms_switch_pt + ms_switch_ot;}
*/

difference()
	{
	union()
		{
		color("DarkSlateGray")
		linear_extrude(height = ms_width)
		
		difference()
			{
				minkowski()
				{	
					square([ms_length,ms_height]);
					circle(r= ms_outer_rad, $fn=20);
				}
			translate([0,0])circle(r= ms_m_hole_rad,$fn=20);
			translate([-ms_m_hole_offset + ms_length,ms_height])circle(r= ms_m_hole_rad,$fn=20);
			translate([ms_m_hole_offset + ms_length,ms_height])circle(r= ms_m_hole_rad,$fn=20);
			}

		color("red")
		translate([ms_switch_pos-ms_switch_length/2,-ms_outer_rad-ms_switch_len,ms_outer_rad])
		linear_extrude(height=ms_switch_width)
			square([ms_switch_length,ms_switch_len]);

		//terminals
		color("silver")
		translate([ms_term_length ,ms_outer_rad + ms_height,ms_outer_rad])
			linear_extrude(height=ms_term_width)
			polygon(points = [
				[0,0],
				[0,ms_term_offset_1_y + ms_term_thick],
				[-ms_term_length,ms_term_offset_1_y + ms_term_thick],
				[-ms_term_length,ms_term_offset_1_y],
				[-ms_term_thick,ms_term_offset_1_y],
				[-ms_term_thick,0]
				]);
		color("silver")
		translate([-ms_outer_rad - ms_term_length,ms_term_offset_2_y,ms_outer_rad])
		linear_extrude(height=ms_term_width)
		square([ms_term_length,ms_term_thick]);
		color("silver")
		translate([-ms_outer_rad - ms_term_length,ms_term_offset_3_y,ms_outer_rad])
		linear_extrude(height=ms_term_width)
		square([ms_term_length,ms_term_thick]);


		//roller lever
		if(ms_roller)
		{
			color("silver")
			translate([ms_roller_offset ,-ms_outer_rad,ms_outer_rad + (ms_roller_width - ms_switch_width)/2])
			linear_extrude(height=ms_switch_width)
			polygon(points = [
				[0,0],
				[0,-ms_switch_ot - ms_roller_thick],
				[ms_roller_length - ms_roller_offset-ms_roller_clip,-ms_switch_ot - ms_roller_thick],
				[ms_roller_length - ms_roller_offset-ms_roller_clip ,-ms_switch_ot - ms_roller_thick-ms_roller_clip],
				[ms_roller_length - ms_roller_offset+ms_roller_clip,-ms_switch_ot - ms_roller_thick-ms_roller_clip],
				[ms_roller_length - ms_roller_offset+ms_roller_clip,-ms_switch_ot - ms_roller_thick],
				[ms_roller_length - ms_roller_offset,-ms_switch_ot],
				[ms_roller_thick,-ms_switch_ot],
				[ms_roller_thick,0]
				]);
			color("black")
			translate([ms_roller_length,-ms_outer_rad-ms_roller_thick*2-ms_roller_rad,ms_outer_rad])
			linear_extrude(height=ms_roller_width)
			circle(r = ms_roller_rad, $fn=20);


		}
		}

	}


}