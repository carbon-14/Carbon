cd ../bin/win32/Release

MaterialCompiler ../../..

REM Level 4
TextureCompiler color -cmp -mipmap ../../.. carbon_c.png
TextureCompiler normal -cmp -mipmap ../../.. carbon_n.png

REM Level 5
TextureCompiler color -cmp -mipmap ../../.. crack_c.png
TextureCompiler normal -cmp -mipmap ../../.. crack_n.png
MeshCompiler -gen_ts -cmp_pos -cmp_vec -cmp_uvs ../../.. sphere.dae
MeshCompiler -cmp_pos -cmp_vec -cmp_uvs ../../.. level5.dae

REM Level 6
TextureCompiler color -cmp -mipmap ../../.. level6/kamen_c.png
TextureCompiler normal -cmp -mipmap ../../.. level6/kamen_n.png
TextureCompiler color -cmp -mipmap ../../.. level6/kamen_stup_c.png
TextureCompiler normal -cmp -mipmap ../../.. level6/kamen_stup_n.png
TextureCompiler color -cmp -mipmap ../../.. level6/marble_c.png
TextureCompiler normal -cmp -mipmap ../../.. level6/marble_n.png
TextureCompiler color -cmp -mipmap ../../.. level6/iron_c.png
TextureCompiler normal -cmp -mipmap ../../.. level6/iron_n.png
TextureCompiler color -cmp -mipmap ../../.. level6/carpet_c.png
TextureCompiler normal -cmp -mipmap ../../.. level6/carpet_n.png
MeshCompiler -gen_ts -cmp_pos -cmp_vec -cmp_uvs ../../.. level6/level6.dae

pause