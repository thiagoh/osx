require "variables";

# Not an error (0)
set "var123456789012345678901234567890" "value";

# Exceed the maximum variable name length (1)
set "var123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" "value";

# Must yield unknown namespace error (no limit exceeded) (1)
set "namespace.sub.sub.variable" "value"; 

# Must yield unknown namespace error (exceeds element limit) (1)
set "namespace.sub.sub.sub.variable" "value";

# Not an error (0)
if string "${32}" "value" {
	stop;
}

# Exceed the maximum match value index (1)
if string "${33}" "value" {
	stop;
}

# Exceed the maximum number of declared variables (1!)
set "var001" "value";
set "var002" "value";
set "var003" "value";
set "var004" "value";
set "var005" "value";
set "var006" "value";
set "var007" "value";
set "var008" "value";
set "var009" "value";
set "var010" "value";
set "var011" "value";
set "var012" "value";
set "var013" "value";
set "var014" "value";
set "var015" "value";
set "var016" "value";
set "var017" "value";
set "var018" "value";
set "var019" "value";
set "var020" "value";
set "var021" "value";
set "var022" "value";
set "var023" "value";
set "var024" "value";
set "var025" "value";
set "var026" "value";
set "var027" "value";
set "var028" "value";
set "var029" "value";
set "var030" "value";
set "var031" "value";
set "var032" "value";
set "var033" "value";
set "var034" "value";
set "var035" "value";
set "var036" "value";
set "var037" "value";
set "var038" "value";
set "var039" "value";
set "var040" "value";
set "var041" "value";
set "var042" "value";
set "var043" "value";
set "var044" "value";
set "var045" "value";
set "var046" "value";
set "var047" "value";
set "var048" "value";
set "var049" "value";
set "var050" "value";
set "var051" "value";
set "var052" "value";
set "var053" "value";
set "var054" "value";
set "var055" "value";
set "var056" "value";
set "var057" "value";
set "var058" "value";
set "var059" "value";
set "var060" "value";
set "var061" "value";
set "var062" "value";
set "var063" "value";
set "var064" "value";
set "var065" "value";
set "var066" "value";
set "var067" "value";
set "var068" "value";
set "var069" "value";
set "var070" "value";
set "var071" "value";
set "var072" "value";
set "var073" "value";
set "var074" "value";
set "var075" "value";
set "var076" "value";
set "var077" "value";
set "var078" "value";
set "var079" "value";
set "var080" "value";
set "var081" "value";
set "var082" "value";
set "var083" "value";
set "var084" "value";
set "var085" "value";
set "var086" "value";
set "var087" "value";
set "var088" "value";
set "var089" "value";
set "var090" "value";
set "var091" "value";
set "var092" "value";
set "var093" "value";
set "var094" "value";
set "var095" "value";
set "var096" "value";
set "var097" "value";
set "var098" "value";
set "var099" "value";

set "var100" "value";
set "var101" "value";
set "var102" "value";
set "var103" "value";
set "var104" "value";
set "var105" "value";
set "var106" "value";
set "var107" "value";
set "var108" "value";
set "var109" "value";
set "var110" "value";
set "var111" "value";
set "var112" "value";
set "var113" "value";
set "var114" "value";
set "var115" "value";
set "var116" "value";
set "var117" "value";
set "var118" "value";
set "var119" "value";
set "var120" "value";
set "var121" "value";
set "var122" "value";
set "var123" "value";
set "var124" "value";
set "var125" "value";
set "var126" "value";
set "var127" "value";
set "var128" "value";
set "var129" "value";
set "var130" "value";
set "var131" "value";
set "var132" "value";
set "var133" "value";
set "var134" "value";
set "var135" "value";
set "var136" "value";
set "var137" "value";
set "var138" "value";
set "var139" "value";
set "var140" "value";
set "var141" "value";
set "var142" "value";
set "var143" "value";
set "var144" "value";
set "var145" "value";
set "var146" "value";
set "var147" "value";
set "var148" "value";
set "var149" "value";
set "var150" "value";
set "var151" "value";
set "var152" "value";
set "var153" "value";
set "var154" "value";
set "var155" "value";
set "var156" "value";
set "var157" "value";
set "var158" "value";
set "var159" "value";
set "var160" "value";
set "var161" "value";
set "var162" "value";
set "var163" "value";
set "var164" "value";
set "var165" "value";
set "var166" "value";
set "var167" "value";
set "var168" "value";
set "var169" "value";
set "var170" "value";
set "var171" "value";
set "var172" "value";
set "var173" "value";
set "var174" "value";
set "var175" "value";
set "var176" "value";
set "var177" "value";
set "var178" "value";
set "var179" "value";
set "var180" "value";
set "var181" "value";
set "var182" "value";
set "var183" "value";
set "var184" "value";
set "var185" "value";
set "var186" "value";
set "var187" "value";
set "var188" "value";
set "var189" "value";
set "var190" "value";
set "var191" "value";
set "var192" "value";
set "var193" "value";
set "var194" "value";
set "var195" "value";
set "var196" "value";
set "var197" "value";
set "var198" "value";
set "var199" "value";
set "var200" "value";

set "var201" "value";
set "var202" "value";
set "var203" "value";
set "var204" "value";
set "var205" "value";
set "var206" "value";
set "var207" "value";
set "var208" "value";
set "var209" "value";
set "var210" "value";
set "var211" "value";
set "var212" "value";
set "var213" "value";
set "var214" "value";
set "var215" "value";
set "var216" "value";
set "var217" "value";
set "var218" "value";
set "var219" "value";
set "var220" "value";
set "var221" "value";
set "var222" "value";
set "var223" "value";
set "var224" "value";
set "var225" "value";
set "var226" "value";
set "var227" "value";
set "var228" "value";
set "var229" "value";
set "var230" "value";
set "var231" "value";
set "var232" "value";
set "var233" "value";
set "var234" "value";
set "var235" "value";
set "var236" "value";
set "var237" "value";
set "var238" "value";
set "var239" "value";
set "var240" "value";
set "var241" "value";
set "var242" "value";
set "var243" "value";
set "var244" "value";
set "var245" "value";
set "var246" "value";
set "var247" "value";
set "var248" "value";
set "var249" "value";
set "var250" "value";
set "var251" "value";
set "var252" "value";
set "var253" "value";
set "var254" "value";
set "var255" "value";
set "var256" "value";
set "var257" "value";
set "var258" "value";
set "var259" "value";
set "var260" "value";
