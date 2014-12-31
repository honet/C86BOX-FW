
# write-addr
# 000~00f 0   SSG
# 010~01d 17  RHYTHM
# 021~0b6 17  FM(1~3)
# 100~110 0   ADPCM
# 121~1b6 17  FM(4~6)

# write-data
# 000~00f    0 SSG
# 010      576 RHYTHM
# 011~01d   83 RHYTHM
# 021~09e   83 FM
# 0a0~0b6   47 FM

# 100~110    0 ADPCM
# 121~19e   83 FM
# 1a0~1b6   47 FM

waitdef86 = [0,17,47,83,576]

#address
def get_addrwait_index(x)
  if (x<=0x0f)
    0
  elsif(x<=0x1d)
    5
  elsif(0x21<=x && x<=0xb6)
    1
  elsif(0x100<=x && x<=0x110)
    0
  elsif(0x121<=x && x<=0x1b6)
    1
  else
    0 #0xf
  end
end

#(0..0xff).each{|x|
#  z = (get_addrwait_index(x*2)<<4) | get_addrwait_index(x*2+1)
#  printf("0x%02x", z)
#  print ","
#  print "\n" if(x%16==15) 
#}

# data
def get_datawait_index(x)
  if (x<=0x0f)
    0
  elsif (x==0x10)
    4
  elsif (0x11<=x && x<=0x1d)
    3
  elsif (0x21<=x && x<=0x9e)
    3
  elsif (0xa0<=x && x<=0xb6)
    2
  elsif (0x100<=x && x<=0x110)
    0
  elsif (0x121<=x && x<=0x19e)
    3
  elsif (0x1a0<=x && x<0x1b6)
    2
  else
    0 #0xf
  end
end

print "\t"
(0..0x1ff).each{ |x|
  z = (get_datawait_index(x)<<4) | get_addrwait_index(x)
  printf("0x%02x", z)
  print ","
  print "\n\t" if(x%16==15) 
}
