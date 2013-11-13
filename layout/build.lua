
srcs = {
	"area.cpp";
	"tight_constraint.cpp";
	"flow_constraint.cpp";
	"form_layout.cpp";
	"box_layout.cpp";
	"grid_layout.cpp";
	"tree_layout.cpp";
}

Library( "layout", Compile( srcs ), LinkLibs( "core" ) );

