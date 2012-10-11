function decorate_page (cur_page, course, prefix, linknames, links)
{
  add_link_sequence(linknames, links);
  add_tabs(cur_page, prefix);
  add_top_rounding_divs();
  add_bottom_rounding_divs();
  add_address(course, prefix);
}

function add_tabs (cur, prefix)
{
  if (!prefix) {
    prefix = '';
  }
  /* If the pages variable is defined, then use it, otherwise use a basic default */
  if (typeof window.pages == "undefined") {
    pages = new Object();
    pages["Home"] = prefix+"index.html";
    pages["Schedule"] = prefix+"schedule.html";
    pages["Policies"] = prefix+"policies.html";
    pages["Resources"] = prefix+"resources.html";
    pages["FAQ"] = prefix+"faq.html";
    pages["OnCourse"] = "https://oncourse.iu.edu/";
  }
  var menu_item;
  var menu_table = document.createElement('table');
  menu_table.setAttribute('id', 'tabs');
  var menu_table_body = document.createElement('tbody');
  menu_table.appendChild(menu_table_body);
  var menu_table_row = document.createElement('tr');
  menu_table_body.appendChild(menu_table_row);
  for (menu_item in window.pages) {
    var menu_table_td = document.createElement('td');
    var link_span = document.createElement('span');
    if (menu_item == cur) {
      menu_table_td.appendChild(create_top_rounding_divs('roundtop_selected'));
      link_span.setAttribute('class','selected');
      link_span.setAttribute('className','selected'); // MS IE B*** S***
      link_span.appendChild(document.createTextNode(menu_item));
    }
    else {
      menu_table_td.appendChild(create_top_rounding_divs('roundtop'));
      var link = document.createElement('a');
      var link_target = window.pages[menu_item];
      if (link_target.substr(0,4) == 'http')
	link.setAttribute('href', link_target);
      else
	link.setAttribute('href', prefix+link_target);
      if (menu_item == 'OnCourse')
	link.setAttribute('target', '_top');
      link.appendChild(document.createTextNode(menu_item));
      link_span.appendChild(link);
    }
    menu_table_td.appendChild(link_span);
    menu_table_row.appendChild(menu_table_td);
  }
  document.getElementsByTagName('body').item(0).insertBefore(menu_table, document.getElementById('content'));
}


function add_top_rounding_divs ()
{
  document.getElementsByTagName('body').item(0).insertBefore(create_top_rounding_divs('roundtop_content'),
							     document.getElementById('content'));
}

function create_top_rounding_divs (rounding_class)
{
  var rounding_div = document.createElement('div');
  rounding_div.setAttribute('class', rounding_class);
  rounding_div.setAttribute('className', rounding_class); // MS IE B*** S***
  var i;
  for (i=1; i <= 5; i++) {
    var new_div = document.createElement('div');
    new_div.setAttribute('class','r'+i.toString());
    new_div.setAttribute('className','r'+i.toString()); // MS IE B*** S***
    rounding_div.appendChild(new_div);
  }
  return rounding_div;
}


function add_bottom_rounding_divs ()
{
  var rounding_div = document.createElement('div');
  rounding_div.setAttribute('class','roundtop_content');
  rounding_div.setAttribute('className','roundtop_content'); // MS IE B*** S***
  var i;
  for (i=5; i >= 1; i--) {
    var new_div = document.createElement('div');
    new_div.setAttribute('class','r'+i.toString());
    new_div.setAttribute('className','r'+i.toString()); // MS IE B*** S***
    rounding_div.appendChild(new_div);
  }
  document.getElementsByTagName('body').item(0).appendChild(rounding_div);
}


function add_address (course, prefix)
{
  if (!prefix)
    prefix = '';
  var addrNode = document.createElement('address');
  var course_url = document.createElement('a');
  course_url.setAttribute('href',prefix+'index.html');
  course_url.appendChild(document.createTextNode(course));
  addrNode.appendChild(course_url);
  addrNode.appendChild(document.createTextNode(' / '));
  var home_url = document.createElement('a');
  home_url.setAttribute('target','_top');
  home_url.setAttribute('href','http://www.cs.indiana.edu/~achauhan/');
  home_url.appendChild(document.createTextNode('Arun Chauhan'));
  addrNode.appendChild(home_url);
  addrNode.appendChild(document.createTextNode(' / '));
  var cs_url = document.createElement('a');
  cs_url.setAttribute('target','_top');
  cs_url.setAttribute('href','http://www.cs.indiana.edu/');
  cs_url.appendChild(document.createTextNode('Computer Science'));
  addrNode.appendChild(cs_url);
  addrNode.appendChild(document.createTextNode(' / '));
  var iu_url = document.createElement('a');
  iu_url.setAttribute('target','_top');
  iu_url.setAttribute('href','http://www.indiana.edu/');
  iu_url.appendChild(document.createTextNode('Indiana University'));
  addrNode.appendChild(iu_url);
  document.getElementsByTagName('body').item(0).appendChild(addrNode);
}

/* We expect matching links for linknames, except for the last name, which (presumably) refers to the page being
   rendered. */
function add_link_sequence (linknames,links)
{
  if (!linknames || !links || (links.length <= 0) || (linknames.length != links.length+1)) {
    return;
  }

  var linksequence_sep = "\u00A0\u00BB\u00A0"; // this is equivalent to HTML &nbsp;&raquo;&nbsp;
  var linksdiv = document.createElement('div');
  linksdiv.setAttribute('id','linksequence');
  linksdiv.appendChild(document.createTextNode('Home'));
  var links_ul = document.createElement('ul');
  linksdiv.appendChild(links_ul);
  var i;
  for (i=0; i < linknames.length-1; i++) {
    var links_item = document.createElement('li');
    links_item.appendChild(document.createTextNode(linksequence_sep));
    var link = document.createElement('a');
    link.appendChild(document.createTextNode(linknames[i]));
    link.setAttribute('href', links[i]);
    links_item.appendChild(link);
    links_ul.appendChild(links_item);
  }
  var links_item = document.createElement('li');
  links_item.appendChild(document.createTextNode(linksequence_sep));
  links_item.appendChild(document.createTextNode(linknames[i]));
  links_ul.appendChild(links_item);
  var content_div = document.getElementById('content');
  content_div.insertBefore(linksdiv, content_div.firstChild);
}
