
// bp_kernel.cl 
// Branch penalty measurement

float funfun(float v) {
	for(int i=0; i<100000; ++i) {
		v = (v*(v+2.0f))/(v+1.9f);
	}
	return v;
}

__kernel __attribute__((reqd_work_group_size (localRange,1,1))) void
branchPenalty(__global float* brancharray, __global float* outarray)
{
	int id = get_global_id(0);
	if(brancharray[id] >= 127.9f && brancharray[id] <= 128.1f) {
	  outarray[id] = funfun(128.0f*brancharray[id]);
	}
	if(brancharray[id] >= 126.9f && brancharray[id] <= 127.1f) {
	  outarray[id] = funfun(127.0f*brancharray[id]);
	}
	if(brancharray[id] >= 125.9f && brancharray[id] <= 126.1f) {
	  outarray[id] = funfun(126.0f*brancharray[id]);
	}
	if(brancharray[id] >= 124.9f && brancharray[id] <= 125.1f) {
	  outarray[id] = funfun(125.0f*brancharray[id]);
	}
	if(brancharray[id] >= 123.9f && brancharray[id] <= 124.1f) {
	  outarray[id] = funfun(124.0f*brancharray[id]);
	}
	if(brancharray[id] >= 122.9f && brancharray[id] <= 123.1f) {
	  outarray[id] = funfun(123.0f*brancharray[id]);
	}
	if(brancharray[id] >= 121.9f && brancharray[id] <= 122.1f) {
	  outarray[id] = funfun(122.0f*brancharray[id]);
	}
	if(brancharray[id] >= 120.9f && brancharray[id] <= 121.1f) {
	  outarray[id] = funfun(121.0f*brancharray[id]);
	}
	if(brancharray[id] >= 119.9f && brancharray[id] <= 120.1f) {
	  outarray[id] = funfun(120.0f*brancharray[id]);
	}
	if(brancharray[id] >= 118.9f && brancharray[id] <= 119.1f) {
	  outarray[id] = funfun(119.0f*brancharray[id]);
	}
	if(brancharray[id] >= 117.9f && brancharray[id] <= 118.1f) {
	  outarray[id] = funfun(118.0f*brancharray[id]);
	}
	if(brancharray[id] >= 116.9f && brancharray[id] <= 117.1f) {
	  outarray[id] = funfun(117.0f*brancharray[id]);
	}
	if(brancharray[id] >= 115.9f && brancharray[id] <= 116.1f) {
	  outarray[id] = funfun(116.0f*brancharray[id]);
	}
	if(brancharray[id] >= 114.9f && brancharray[id] <= 115.1f) {
	  outarray[id] = funfun(115.0f*brancharray[id]);
	}
	if(brancharray[id] >= 113.9f && brancharray[id] <= 114.1f) {
	  outarray[id] = funfun(114.0f*brancharray[id]);
	}
	if(brancharray[id] >= 112.9f && brancharray[id] <= 113.1f) {
	  outarray[id] = funfun(113.0f*brancharray[id]);
	}
	if(brancharray[id] >= 111.9f && brancharray[id] <= 112.1f) {
	  outarray[id] = funfun(112.0f*brancharray[id]);
	}
	if(brancharray[id] >= 110.9f && brancharray[id] <= 111.1f) {
	  outarray[id] = funfun(111.0f*brancharray[id]);
	}
	if(brancharray[id] >= 109.9f && brancharray[id] <= 110.1f) {
	  outarray[id] = funfun(110.0f*brancharray[id]);
	}
	if(brancharray[id] >= 108.9f && brancharray[id] <= 109.1f) {
	  outarray[id] = funfun(109.0f*brancharray[id]);
	}
	if(brancharray[id] >= 107.9f && brancharray[id] <= 108.1f) {
	  outarray[id] = funfun(108.0f*brancharray[id]);
	}
	if(brancharray[id] >= 106.9f && brancharray[id] <= 107.1f) {
	  outarray[id] = funfun(107.0f*brancharray[id]);
	}
	if(brancharray[id] >= 105.9f && brancharray[id] <= 106.1f) {
	  outarray[id] = funfun(106.0f*brancharray[id]);
	}
	if(brancharray[id] >= 104.9f && brancharray[id] <= 105.1f) {
	  outarray[id] = funfun(105.0f*brancharray[id]);
	}
	if(brancharray[id] >= 103.9f && brancharray[id] <= 104.1f) {
	  outarray[id] = funfun(104.0f*brancharray[id]);
	}
	if(brancharray[id] >= 102.9f && brancharray[id] <= 103.1f) {
	  outarray[id] = funfun(103.0f*brancharray[id]);
	}
	if(brancharray[id] >= 101.9f && brancharray[id] <= 102.1f) {
	  outarray[id] = funfun(102.0f*brancharray[id]);
	}
	if(brancharray[id] >= 100.9f && brancharray[id] <= 101.1f) {
	  outarray[id] = funfun(101.0f*brancharray[id]);
	}
	if(brancharray[id] >= 99.9f && brancharray[id] <= 100.1f) {
	  outarray[id] = funfun(100.0f*brancharray[id]);
	}
	if(brancharray[id] >= 98.9f && brancharray[id] <= 99.1f) {
	  outarray[id] = funfun(99.0f*brancharray[id]);
	}
	if(brancharray[id] >= 97.9f && brancharray[id] <= 98.1f) {
	  outarray[id] = funfun(98.0f*brancharray[id]);
	}
	if(brancharray[id] >= 96.9f && brancharray[id] <= 97.1f) {
	  outarray[id] = funfun(97.0f*brancharray[id]);
	}
	if(brancharray[id] >= 95.9f && brancharray[id] <= 96.1f) {
	  outarray[id] = funfun(96.0f*brancharray[id]);
	}
	if(brancharray[id] >= 94.9f && brancharray[id] <= 95.1f) {
	  outarray[id] = funfun(95.0f*brancharray[id]);
	}
	if(brancharray[id] >= 93.9f && brancharray[id] <= 94.1f) {
	  outarray[id] = funfun(94.0f*brancharray[id]);
	}
	if(brancharray[id] >= 92.9f && brancharray[id] <= 93.1f) {
	  outarray[id] = funfun(93.0f*brancharray[id]);
	}
	if(brancharray[id] >= 91.9f && brancharray[id] <= 92.1f) {
	  outarray[id] = funfun(92.0f*brancharray[id]);
	}
	if(brancharray[id] >= 90.9f && brancharray[id] <= 91.1f) {
	  outarray[id] = funfun(91.0f*brancharray[id]);
	}
	if(brancharray[id] >= 89.9f && brancharray[id] <= 90.1f) {
	  outarray[id] = funfun(90.0f*brancharray[id]);
	}
	if(brancharray[id] >= 88.9f && brancharray[id] <= 89.1f) {
	  outarray[id] = funfun(89.0f*brancharray[id]);
	}
	if(brancharray[id] >= 87.9f && brancharray[id] <= 88.1f) {
	  outarray[id] = funfun(88.0f*brancharray[id]);
	}
	if(brancharray[id] >= 86.9f && brancharray[id] <= 87.1f) {
	  outarray[id] = funfun(87.0f*brancharray[id]);
	}
	if(brancharray[id] >= 85.9f && brancharray[id] <= 86.1f) {
	  outarray[id] = funfun(86.0f*brancharray[id]);
	}
	if(brancharray[id] >= 84.9f && brancharray[id] <= 85.1f) {
	  outarray[id] = funfun(85.0f*brancharray[id]);
	}
	if(brancharray[id] >= 83.9f && brancharray[id] <= 84.1f) {
	  outarray[id] = funfun(84.0f*brancharray[id]);
	}
	if(brancharray[id] >= 82.9f && brancharray[id] <= 83.1f) {
	  outarray[id] = funfun(83.0f*brancharray[id]);
	}
	if(brancharray[id] >= 81.9f && brancharray[id] <= 82.1f) {
	  outarray[id] = funfun(82.0f*brancharray[id]);
	}
	if(brancharray[id] >= 80.9f && brancharray[id] <= 81.1f) {
	  outarray[id] = funfun(81.0f*brancharray[id]);
	}
	if(brancharray[id] >= 79.9f && brancharray[id] <= 80.1f) {
	  outarray[id] = funfun(80.0f*brancharray[id]);
	}
	if(brancharray[id] >= 78.9f && brancharray[id] <= 79.1f) {
	  outarray[id] = funfun(79.0f*brancharray[id]);
	}
	if(brancharray[id] >= 77.9f && brancharray[id] <= 78.1f) {
	  outarray[id] = funfun(78.0f*brancharray[id]);
	}
	if(brancharray[id] >= 76.9f && brancharray[id] <= 77.1f) {
	  outarray[id] = funfun(77.0f*brancharray[id]);
	}
	if(brancharray[id] >= 75.9f && brancharray[id] <= 76.1f) {
	  outarray[id] = funfun(76.0f*brancharray[id]);
	}
	if(brancharray[id] >= 74.9f && brancharray[id] <= 75.1f) {
	  outarray[id] = funfun(75.0f*brancharray[id]);
	}
	if(brancharray[id] >= 73.9f && brancharray[id] <= 74.1f) {
	  outarray[id] = funfun(74.0f*brancharray[id]);
	}
	if(brancharray[id] >= 72.9f && brancharray[id] <= 73.1f) {
	  outarray[id] = funfun(73.0f*brancharray[id]);
	}
	if(brancharray[id] >= 71.9f && brancharray[id] <= 72.1f) {
	  outarray[id] = funfun(72.0f*brancharray[id]);
	}
	if(brancharray[id] >= 70.9f && brancharray[id] <= 71.1f) {
	  outarray[id] = funfun(71.0f*brancharray[id]);
	}
	if(brancharray[id] >= 69.9f && brancharray[id] <= 70.1f) {
	  outarray[id] = funfun(70.0f*brancharray[id]);
	}
	if(brancharray[id] >= 68.9f && brancharray[id] <= 69.1f) {
	  outarray[id] = funfun(69.0f*brancharray[id]);
	}
	if(brancharray[id] >= 67.9f && brancharray[id] <= 68.1f) {
	  outarray[id] = funfun(68.0f*brancharray[id]);
	}
	if(brancharray[id] >= 66.9f && brancharray[id] <= 67.1f) {
	  outarray[id] = funfun(67.0f*brancharray[id]);
	}
	if(brancharray[id] >= 65.9f && brancharray[id] <= 66.1f) {
	  outarray[id] = funfun(66.0f*brancharray[id]);
	}
	if(brancharray[id] >= 64.9f && brancharray[id] <= 65.1f) {
	  outarray[id] = funfun(65.0f*brancharray[id]);
	}
	if(brancharray[id] >= 63.9f && brancharray[id] <= 64.1f) {
	  outarray[id] = funfun(64.0f*brancharray[id]);
	}
	if(brancharray[id] >= 62.9f && brancharray[id] <= 63.1f) {
	  outarray[id] = funfun(63.0f*brancharray[id]);
	}
	if(brancharray[id] >= 61.9f && brancharray[id] <= 62.1f) {
	  outarray[id] = funfun(62.0f*brancharray[id]);
	}
	if(brancharray[id] >= 60.9f && brancharray[id] <= 61.1f) {
	  outarray[id] = funfun(61.0f*brancharray[id]);
	}
	if(brancharray[id] >= 59.9f && brancharray[id] <= 60.1f) {
	  outarray[id] = funfun(60.0f*brancharray[id]);
	}
	if(brancharray[id] >= 58.9f && brancharray[id] <= 59.1f) {
	  outarray[id] = funfun(59.0f*brancharray[id]);
	}
	if(brancharray[id] >= 57.9f && brancharray[id] <= 58.1f) {
	  outarray[id] = funfun(58.0f*brancharray[id]);
	}
	if(brancharray[id] >= 56.9f && brancharray[id] <= 57.1f) {
	  outarray[id] = funfun(57.0f*brancharray[id]);
	}
	if(brancharray[id] >= 55.9f && brancharray[id] <= 56.1f) {
	  outarray[id] = funfun(56.0f*brancharray[id]);
	}
	if(brancharray[id] >= 54.9f && brancharray[id] <= 55.1f) {
	  outarray[id] = funfun(55.0f*brancharray[id]);
	}
	if(brancharray[id] >= 53.9f && brancharray[id] <= 54.1f) {
	  outarray[id] = funfun(54.0f*brancharray[id]);
	}
	if(brancharray[id] >= 52.9f && brancharray[id] <= 53.1f) {
	  outarray[id] = funfun(53.0f*brancharray[id]);
	}
	if(brancharray[id] >= 51.9f && brancharray[id] <= 52.1f) {
	  outarray[id] = funfun(52.0f*brancharray[id]);
	}
	if(brancharray[id] >= 50.9f && brancharray[id] <= 51.1f) {
	  outarray[id] = funfun(51.0f*brancharray[id]);
	}
	if(brancharray[id] >= 49.9f && brancharray[id] <= 50.1f) {
	  outarray[id] = funfun(50.0f*brancharray[id]);
	}
	if(brancharray[id] >= 48.9f && brancharray[id] <= 49.1f) {
	  outarray[id] = funfun(49.0f*brancharray[id]);
	}
	if(brancharray[id] >= 47.9f && brancharray[id] <= 48.1f) {
	  outarray[id] = funfun(48.0f*brancharray[id]);
	}
	if(brancharray[id] >= 46.9f && brancharray[id] <= 47.1f) {
	  outarray[id] = funfun(47.0f*brancharray[id]);
	}
	if(brancharray[id] >= 45.9f && brancharray[id] <= 46.1f) {
	  outarray[id] = funfun(46.0f*brancharray[id]);
	}
	if(brancharray[id] >= 44.9f && brancharray[id] <= 45.1f) {
	  outarray[id] = funfun(45.0f*brancharray[id]);
	}
	if(brancharray[id] >= 43.9f && brancharray[id] <= 44.1f) {
	  outarray[id] = funfun(44.0f*brancharray[id]);
	}
	if(brancharray[id] >= 42.9f && brancharray[id] <= 43.1f) {
	  outarray[id] = funfun(43.0f*brancharray[id]);
	}
	if(brancharray[id] >= 41.9f && brancharray[id] <= 42.1f) {
	  outarray[id] = funfun(42.0f*brancharray[id]);
	}
	if(brancharray[id] >= 40.9f && brancharray[id] <= 41.1f) {
	  outarray[id] = funfun(41.0f*brancharray[id]);
	}
	if(brancharray[id] >= 39.9f && brancharray[id] <= 40.1f) {
	  outarray[id] = funfun(40.0f*brancharray[id]);
	}
	if(brancharray[id] >= 38.9f && brancharray[id] <= 39.1f) {
	  outarray[id] = funfun(39.0f*brancharray[id]);
	}
	if(brancharray[id] >= 37.9f && brancharray[id] <= 38.1f) {
	  outarray[id] = funfun(38.0f*brancharray[id]);
	}
	if(brancharray[id] >= 36.9f && brancharray[id] <= 37.1f) {
	  outarray[id] = funfun(37.0f*brancharray[id]);
	}
	if(brancharray[id] >= 35.9f && brancharray[id] <= 36.1f) {
	  outarray[id] = funfun(36.0f*brancharray[id]);
	}
	if(brancharray[id] >= 34.9f && brancharray[id] <= 35.1f) {
	  outarray[id] = funfun(35.0f*brancharray[id]);
	}
	if(brancharray[id] >= 33.9f && brancharray[id] <= 34.1f) {
	  outarray[id] = funfun(34.0f*brancharray[id]);
	}
	if(brancharray[id] >= 32.9f && brancharray[id] <= 33.1f) {
	  outarray[id] = funfun(33.0f*brancharray[id]);
	}
	if(brancharray[id] >= 31.9f && brancharray[id] <= 32.1f) {
	  outarray[id] = funfun(32.0f*brancharray[id]);
	}
	if(brancharray[id] >= 30.9f && brancharray[id] <= 31.1f) {
	  outarray[id] = funfun(31.0f*brancharray[id]);
	}
	if(brancharray[id] >= 29.9f && brancharray[id] <= 30.1f) {
	  outarray[id] = funfun(30.0f*brancharray[id]);
	}
	if(brancharray[id] >= 28.9f && brancharray[id] <= 29.1f) {
	  outarray[id] = funfun(29.0f*brancharray[id]);
	}
	if(brancharray[id] >= 27.9f && brancharray[id] <= 28.1f) {
	  outarray[id] = funfun(28.0f*brancharray[id]);
	}
	if(brancharray[id] >= 26.9f && brancharray[id] <= 27.1f) {
	  outarray[id] = funfun(27.0f*brancharray[id]);
	}
	if(brancharray[id] >= 25.9f && brancharray[id] <= 26.1f) {
	  outarray[id] = funfun(26.0f*brancharray[id]);
	}
	if(brancharray[id] >= 24.9f && brancharray[id] <= 25.1f) {
	  outarray[id] = funfun(25.0f*brancharray[id]);
	}
	if(brancharray[id] >= 23.9f && brancharray[id] <= 24.1f) {
	  outarray[id] = funfun(24.0f*brancharray[id]);
	}
	if(brancharray[id] >= 22.9f && brancharray[id] <= 23.1f) {
	  outarray[id] = funfun(23.0f*brancharray[id]);
	}
	if(brancharray[id] >= 21.9f && brancharray[id] <= 22.1f) {
	  outarray[id] = funfun(22.0f*brancharray[id]);
	}
	if(brancharray[id] >= 20.9f && brancharray[id] <= 21.1f) {
	  outarray[id] = funfun(21.0f*brancharray[id]);
	}
	if(brancharray[id] >= 19.9f && brancharray[id] <= 20.1f) {
	  outarray[id] = funfun(20.0f*brancharray[id]);
	}
	if(brancharray[id] >= 18.9f && brancharray[id] <= 19.1f) {
	  outarray[id] = funfun(19.0f*brancharray[id]);
	}
	if(brancharray[id] >= 17.9f && brancharray[id] <= 18.1f) {
	  outarray[id] = funfun(18.0f*brancharray[id]);
	}
	if(brancharray[id] >= 16.9f && brancharray[id] <= 17.1f) {
	  outarray[id] = funfun(17.0f*brancharray[id]);
	}
	if(brancharray[id] >= 15.9f && brancharray[id] <= 16.1f) {
	  outarray[id] = funfun(16.0f*brancharray[id]);
	}
	if(brancharray[id] >= 14.9f && brancharray[id] <= 15.1f) {
	  outarray[id] = funfun(15.0f*brancharray[id]);
	}
	if(brancharray[id] >= 13.9f && brancharray[id] <= 14.1f) {
	  outarray[id] = funfun(14.0f*brancharray[id]);
	}
	if(brancharray[id] >= 12.9f && brancharray[id] <= 13.1f) {
	  outarray[id] = funfun(13.0f*brancharray[id]);
	}
	if(brancharray[id] >= 11.9f && brancharray[id] <= 12.1f) {
	  outarray[id] = funfun(12.0f*brancharray[id]);
	}
	if(brancharray[id] >= 10.9f && brancharray[id] <= 11.1f) {
	  outarray[id] = funfun(11.0f*brancharray[id]);
	}
	if(brancharray[id] >= 9.9f && brancharray[id] <= 10.1f) {
	  outarray[id] = funfun(10.0f*brancharray[id]);
	}
	if(brancharray[id] >= 8.9f && brancharray[id] <= 9.1f) {
	  outarray[id] = funfun(9.0f*brancharray[id]);
	}
	if(brancharray[id] >= 7.9f && brancharray[id] <= 8.1f) {
	  outarray[id] = funfun(8.0f*brancharray[id]);
	}
	if(brancharray[id] >= 6.9f && brancharray[id] <= 7.1f) {
	  outarray[id] = funfun(7.0f*brancharray[id]);
	}
	if(brancharray[id] >= 5.9f && brancharray[id] <= 6.1f) {
	  outarray[id] = funfun(6.0f*brancharray[id]);
	}
	if(brancharray[id] >= 4.9f && brancharray[id] <= 5.1f) {
	  outarray[id] = funfun(5.0f*brancharray[id]);
	}
	if(brancharray[id] >= 3.9f && brancharray[id] <= 4.1f) {
	  outarray[id] = funfun(4.0f*brancharray[id]);
	}
	if(brancharray[id] >= 2.9f && brancharray[id] <= 3.1f) {
	  outarray[id] = funfun(3.0f*brancharray[id]);
	}
	if(brancharray[id] >= 1.9f && brancharray[id] <= 2.1f) {
	  outarray[id] = funfun(2.0f*brancharray[id]);
	}
	if(brancharray[id] >= 0.9f && brancharray[id] <= 1.1f) {
	  outarray[id] = funfun(1.0f*brancharray[id]);
	}
	//if(id == 0)
	//	outarray[3] = 5.0f;
}
