/** 
 *  VIBesFigPaving class
 * ----------------------------------------------------------------------------
 *  \date       2016
 *  \author     Simon Rohou
 *  \copyright  Copyright 2020 Simon Rohou
 *  \license    This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 */

#include "tubex_VIBesFigPaving.h"

using namespace std;
using namespace ibex;

namespace tubex
{
  VIBesFigPaving::VIBesFigPaving(const string& fig_name, const Paving *paving)
    : VIBesFig(fig_name), m_paving(paving)
  {
    assert(paving != NULL);

    set_properties(100, 100, 500, 500); // default properties
    axis_limits(paving->box());

    // Default color map
    map<int,string> color_map;
    color_map[VALUE_IN] = "#9C9C9C[green]";
    color_map[VALUE_MAYBE] = "#9C9C9C[yellow]";
    color_map[VALUE_OUT] = "#9C9C9C[cyan]";
    set_color_map(color_map);
  }

  void VIBesFigPaving::set_color_map(const map<int,string>& color_map)
  {
    // todo: deal with color maps defined with any kind of values
    vibes::newGroup("val_in", color_map.at(VALUE_IN), vibesParams("figure", name()));
    vibes::newGroup("val_maybe", color_map.at(VALUE_MAYBE), vibesParams("figure", name()));
    vibes::newGroup("val_out", color_map.at(VALUE_OUT), vibesParams("figure", name()));
  }

  void VIBesFigPaving::show()
  {
    // todo: deal with color maps defined with any kind of values
    vibes::clearGroup(name(), "val_in");
    vibes::clearGroup(name(), "val_maybe");
    vibes::clearGroup(name(), "val_out");
    draw_paving(m_paving);
  }

  void VIBesFigPaving::draw_paving(const Paving *paving)
  {
    assert(paving != NULL);
    
    if(paving->is_leaf())
    {
      string color_group;
      switch(paving->value())
      {
        case VALUE_IN:
          color_group = "val_in";
          break;

        case VALUE_OUT:
          color_group = "val_out";
          break;

        case VALUE_MAYBE:
        default:
          color_group = "val_maybe";
      }

      draw_box(paving->box(), vibesParams("figure", name(), "group", color_group));
    }

    else
    {
      draw_paving(paving->get_first_subpaving());
      draw_paving(paving->get_second_subpaving());
    }
  }
}