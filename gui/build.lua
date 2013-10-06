
srcs = {
	"area.cpp";
	"container.cpp";
--	"constraint.cpp";
	"tight_constraint.cpp";
	"flow_constraint.cpp";
	"simple_controller.cpp";
}

Library( "gui", Compile( srcs ) );

