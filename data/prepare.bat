cd ../bin/win32/Release
TextureCompiler color -cmp ../../../cache/carbon_c.btx ../../../data/carbon_c.png
TextureCompiler normal -cmp ../../../cache/carbon_n.btx ../../../data/carbon_n.png

MeshCompiler -cmp_pos -cmp_vec -cmp_uvs ../../../cache/sibenik.bmh ../../../data/sibenik.dae

pause