var tests=[{ name: 'u32toa', info: 'release win64 vc2019', categories: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 'URND LEN'], data: [
{ name: 'fmt', data:[4.750000,6.000000,7.000000,7.250000,9.500000,9.500000,12.500000,12.250000,15.500000,15.000000,32.750000,]},
{ name: 'jeaiii::to_text', data:[4.000000,4.000000,5.500000,5.500000,6.250000,6.250000,7.750000,7.750000,9.500000,9.500000,20.000000,]},
{ name: 'int_to_chars_jeaiii<void>', data:[4.000000,4.250000,6.000000,6.000000,6.750000,6.750000,8.000000,8.250000,9.250000,9.500000,26.500000,]},
{ name: 'naive', data:[9.000000,13.000000,17.000000,20.500000,24.500000,28.500000,32.500000,37.250000,42.000000,47.000000,51.000000,]},
{ name: 'null', data:[3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,]},
{ name: 'unrolledlut', data:[7.250000,6.500000,8.750000,7.750000,9.000000,9.000000,8.750000,8.750000,10.750000,10.750000,26.500000,]},
]},
{ name: 'u64toa', info: 'release win64 vc2019', categories: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 'URND LEN'], data: [
{ name: 'fmt', data:[6.500000,5.750000,8.500000,8.750000,11.750000,11.500000,16.000000,15.500000,21.000000,20.000000,25.750000,25.250000,31.250000,30.500000,37.000000,36.250000,42.750000,41.500000,48.250000,47.250000,56.500000,]},
{ name: 'jeaiii::to_text', data:[4.000000,4.000000,6.000000,6.000000,7.250000,7.250000,8.000000,8.000000,10.000000,10.000000,11.750000,11.750000,13.000000,13.000000,14.250000,14.250000,16.000000,16.000000,17.000000,17.000000,32.000000,]},
{ name: 'int_to_chars_jeaiii<void>', data:[4.000000,4.250000,6.000000,6.750000,7.000000,7.750000,8.500000,8.750000,10.250000,10.250000,13.000000,12.500000,14.000000,14.250000,15.500000,15.250000,17.500000,17.500000,18.250000,18.250000,41.750000,]},
{ name: 'naive', data:[9.250000,12.750000,16.500000,20.250000,23.750000,27.500000,31.750000,36.000000,40.250000,45.000000,50.000000,55.000000,60.000000,65.250000,70.000000,75.250000,80.000000,85.000000,90.000000,95.000000,82.250000,]},
{ name: 'null', data:[3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,3.250000,]},
{ name: 'unrolledlut', data:[7.750000,7.250000,9.500000,8.250000,10.250000,10.250000,10.250000,10.250000,11.750000,11.750000,17.500000,17.500000,19.750000,19.750000,19.750000,19.750000,22.750000,22.750000,25.000000,24.750000,42.500000,]},
]},
];
