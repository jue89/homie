const {Box} = require('quickbox');
const {TR, BR, BL, TL, CC, BC} = require('quickbox/alignment');
const {rectangle, circle, fromPoints} = require('quickbox/elements');

const connector = [
	circle({radius: 1.7, center: [0, 20]}),
	rectangle({size: [24.8, 32.5], center: [0, 0]}),
	circle({radius: 1.7, center: [0, -20]})
];

const box = new Box({
	/* All units in mm */
	tolerances: {
		/* 3D prints aren't perfect.
		 * The oversize option adds a gap at every point the 3D print
		 * touches any other object like jacks, PCB, ... */
		oversize: 0.3
	},
	box: {
		/* Box radius */
		radius: 6,

		/* Wall thickness. Make sure to select a multiple of the 3D
		 * printer's nozzle diameter for best printing results. */
		wall: 2.4,

		/* Box height */
		height: 60,

		/* Box screws: */
		boxScrew: {
			/* Diameter of the thread insert */
			threadInsertDiameter: 3.6,

			/* Height of the thread insert */
			threadInsertHeight: 6.0,

			/* Screw diameter */
			screwDiameter: 3,

			/* Screw head diameter */
			screwHeadDiameter: 5.6
		},

		/* Inner PCB mounting screws: */
		mountingScrew: {
			/* Diameter of the thread insert */
			threadInsertDiameter: 3.6,

			/* Height of the thread insert */
			threadInsertHeight: 6.0,

			/* Diameter of the dome the PCB is resting on */
			domeDiameter: 8
		}
	},
	pcb: {
		/* Size of the rectangular PCB */
		size: [55.88, 46.99],

		/* PCB thickness */
		thickness: 1.5,

		/* Padding measured from the PCB edge to the outer box edge
		 *        N   E     S   W */
		padding: [50, 6.48, 15, 80]
	},
	mounts: [
		/* Center positions of the PCB mounts.
		 * The left bottom corner of the PCB is point [0, 0] */
		[35.56, 3.81],
		[35.56, 43.18],
	],
	breakouts: [{
		face: 'n',
		anchor: 'center',
		sketch: {
			j5_x: CC([-45, 0], connector),
			j6_x: CC([-15, 0], connector),
			j7_x: CC([15, 0], connector),
			j8_x: CC([45, 0], connector)
		}
	}, {
		face: 's',
		anchor: 'center',
		sketch: {
			jpwr_x: CC([-42, 0], [
				circle({radius: 1.7, center: [0, 18]}),
				rectangle({size: [31.1, 27.3], center: [0, 0]}),
				circle({radius: 1.7, center: [0, -18]})
			]),
		}
	}, {
		face: 'e',
		anchor: 'pcb-left',
		sketch: {
			j4: BC([6.35, 0], rectangle({size: [11.7, 10.5]})),
			j3: BC([24.13, 0.9], rectangle({size: [11.5, 15.3]})),
			j1: BC([40.64, 0.9], rectangle({size: [11.5, 15.3]}))
		}
	}]
});

const base = __filename.substr(0, __filename.length - 3);
box.createBaseSTL(`${base}-base.stl`);
box.createCapSTL(`${base}-cap.stl`);
