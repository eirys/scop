/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_loader.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 10:12:47 by eli               #+#    #+#             */
/*   Updated: 2023/05/12 19:06:27 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMAGE_LOADER_HPP
# define IMAGE_LOADER_HPP

// Std
# include <string>
# include <vector>

# include "image_handler.hpp"

namespace scop {

enum ImageType {
	PPM,
	// PNG,		TODO
	// JPEG,
	// BMP,
	// TGA,
};

/**
 * Image loader interface.
 * 
 * The files are loaded into a vector of char, which is then parsed.
 * This format is necessary to be able to load binary files (images).
*/
class ImageLoader {
public:
	/* ========================================================================= */
	/*                                  TYPEDEF                                  */
	/* ========================================================================= */

	typedef		enum ImageType	ImageType;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	virtual ~ImageLoader() = default;
	virtual scop::Image		load() = 0;

protected:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	const std::string	path;		// File path
	ImageType			type;		// File extension
	std::vector<char>	data;		// Contains file entire content
	size_t				width;
	size_t				height;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	ImageLoader(const char* path, ImageType type): path(path), type(type) {}

	ImageLoader() = delete;
	ImageLoader(const ImageLoader& x) = delete;
	ImageLoader(ImageLoader&& x) = delete;
	ImageLoader&	operator=(const ImageLoader& x) = delete;

	/* ========================================================================= */
	/*                                 EXCEPTIONS                                */
	/* ========================================================================= */

	class FailedToLoadImage: public std::exception {
	public:
		FailedToLoadImage() = delete;
		FailedToLoadImage(const std::string& path, const std::string& spec):
			spec("Failed to load `" + path + "`: " + spec) {}

		const char*	what() const noexcept override {
			return spec.c_str();
		}
	private:
		const std::string	spec;
	};

}; // class ImageLoader

} // namespace scop

#endif